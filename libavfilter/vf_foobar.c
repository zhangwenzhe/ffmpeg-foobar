

/**
 * @file
 * object track filter
 *
 */
#include <signal.h>
#include "libavutil/avassert.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h" 
#include "libavutil/opt.h"
#include "avfilter.h"
#include "formats.h"
#include "internal.h"
#include "video.h"
#include <mymatcher.h>
#include <zffqueue.h>


#define SCALE_TIME 7
#define SCALE_RANGE 3

#define DCT_W 640
#define DCT_H 480

static struct SwsContext *swsc = NULL;
static void * dtcbuf = NULL;


static int zswitch2=0; //switch that if detect is enabled
static uint8_t *rgb = NULL;
static void * matcher = NULL;
static int scale=0;
static int target=0;

#define NUM_XY 120

static int x[NUM_XY];
static int y[NUM_XY];
static int num = 0;
static int vnum = 0;
static int pxfmt = 0;
static int w = 0;
static int h = 0;
static int d = 0;
static int zfq_flag = 0;

enum FilterMode {
    MODE_NORMAL = 0,
    MODE_AUTOTRIGGER
};

typedef struct FoobarContext {
    const AVClass *class;
    int mode;
} FoobarContext;

#define OFFSET(x) offsetof(FoobarContext, x)
#define FLAGS AV_OPT_FLAG_FILTERING_PARAM|AV_OPT_FLAG_VIDEO_PARAM
static const AVOption foobar_options[] = {
    { "mode", "set mode", OFFSET(mode), AV_OPT_TYPE_INT, {.i64=MODE_NORMAL}, 0, 1, FLAGS, "mode" },
    { NULL }
};

AVFILTER_DEFINE_CLASS(foobar);

static int min(int a, int b){
	return a?a<b:b;
}

static void do_submit(){
	char cmdbuf[4096];
	char tmp[4096];
	sprintf(cmdbuf, "/home/zwz/collector/post-data-fire.py %d ", num);
	int i;
	for(i = 0; i < vnum; i++){
		memcpy(tmp, cmdbuf, 4096);
		if(i != vnum - 1){
			sprintf(cmdbuf, "%s%d,", tmp, x[i]);
		}else{
			sprintf(cmdbuf, "%s%d ", tmp, x[i]);
		}
	}
	for(i = 0; i < vnum; i++){
		memcpy(tmp, cmdbuf, 4096);
		if(i != vnum - 1){
			sprintf(cmdbuf, "%s%d,", tmp, y[i]);
		}else{
			sprintf(cmdbuf, "%s%d", tmp, y[i]);
		}
	}

	if(target){
		strcat(cmdbuf, " yes &");
	}else{
		strcat(cmdbuf, " no &");
	}

	system(cmdbuf);
	av_log(NULL, AV_LOG_INFO, "%s\n", cmdbuf);
}

static void
sigterm_handlerz2(int sig)
{
	if(!zswitch2){//begin to track	
		scale = 0;
		zswitch2 = 1;
		num = 0;
		vnum = 0;
	}else{//end track
		if(target){
			scale = 0;
		}
		do_submit();
		zswitch2 = 0;
	}
	signal(SIGTSTP , sigterm_handlerz2);
}


static av_cold int init(AVFilterContext *ctx)
{
	const FoobarContext * foobar = ctx->priv;
	signal(SIGTSTP , sigterm_handlerz2);
	
	av_log(NULL, AV_LOG_INFO, "autotrigger %d\n", foobar->mode);
	
    return 0;
}

static int query_formats(AVFilterContext *ctx)
{
    const FoobarContext *foobar = ctx->priv;
    static const enum AVPixelFormat wires_pix_fmts[] = {AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE};
    static const enum AVPixelFormat canny_pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUV422P, AV_PIX_FMT_YUV444P, AV_PIX_FMT_GBRP, AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE};
    static const enum AVPixelFormat colormix_pix_fmts[] = {AV_PIX_FMT_GBRP, AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE};
    AVFilterFormats *fmts_list;

    fmts_list = ff_make_format_list(canny_pix_fmts);
    if (!fmts_list)
        return AVERROR(ENOMEM);
    return ff_set_common_formats(ctx, fmts_list);
}



static void draw_vline_GRAY8(AVFrame *in, int x, int yfrom, int yto){
	int i;
	for(i=yfrom; i<yto; i++){
		in->data[0][i*(in->width) + x] = 255;
	}
}

static void draw_vline_YUV420P(AVFrame *in, int x, int yfrom, int yto){
	int i;
	for(i=yfrom/2; i<yto/2; i++){
		in->data[1][i*(in->width/2) + x/2] = 0;
		in->data[2][i*(in->width/2) + x/2] = 0;
	}
}

static void print_once(int pxfmt){
	static int one = 1;
	if(one){
		av_log(NULL, AV_LOG_INFO, "WARNING: unsupported pix format %d, only draw grey box\n", pxfmt); 
		one = 0;
	}
	
}

static void draw_vline(AVFrame *in, int x, int yfrom, int yto){
	return draw_vline_GRAY8(in, x, yfrom, yto);
	if(pxfmt == AV_PIX_FMT_GRAY8){
		return draw_vline_GRAY8(in, x, yfrom, yto);
	}else if(pxfmt == AV_PIX_FMT_YUV420P){
		return draw_vline_YUV420P(in, x, yfrom, yto);
	}else{
		//av_log(NULL, AV_LOG_INFO, "unsupported pix format %d\n", pxfmt); 
		print_once(pxfmt);
		return draw_vline_GRAY8(in, x, yfrom, yto);
	}
}

static void draw_pline_GRAY8(AVFrame *in, int y, int xfrom, int xto){
	int i;
	for(i=xfrom; i<xto; i++){
		in->data[0][y*in->width + i] = 255;
	}
}

static void draw_pline_YUV420P(AVFrame *in, int y, int xfrom, int xto){
	int i;
	for(i=xfrom/2; i<xto/2; i++){
		in->data[1][(y/2)*(in->width/2) + i] = 0;
		in->data[2][(y/2)*(in->width/2) + i] = 0;
	}
}

static void draw_pline(AVFrame *in, int y, int xfrom, int xto){
	return draw_pline_GRAY8(in, y, xfrom, xto);
	if(pxfmt == AV_PIX_FMT_GRAY8){
		return draw_pline_GRAY8(in, y, xfrom, xto);
	}else if(pxfmt == AV_PIX_FMT_YUV420P){
		return draw_pline_YUV420P(in, y, xfrom, xto);
	}else{
		//av_log(NULL, AV_LOG_INFO, "unsupported pix format %d\n", pxfmt); 
		print_once(pxfmt);
		return draw_pline_GRAY8(in, y, xfrom, xto);
	}
}

//static void draw_box_grey(AVFrame *in, int x, int y, int w, int h){
	//int color = 255;
	//
	//if(x<=0 || x+w >= in->width || y<=0 || y+h >= in->height ){
		//return;
	//}
	//
	//if(in->width/2 >= x && in->width/2 <= x+w && in->height/2 >= y && in->height/2 <= y+h){
		//target=1;
	//}else{
		//target=0;
	//}
//
	////av_log(NULL, AV_LOG_INFO, "draw_box x %d, y %d, w %d, h %d, rx %d, ry %d\n", 
		////y, x, w, h, x+w, y+h);
	//
	////draw_pline_y(in, y, x, x+w, 255);
	////draw_pline_y(in, y+h, x, x+w, 255);
	//
	//draw_pline_y(in, y, x, x+w, color);
	//draw_pline_y(in, y+h, x, x+w, color);
	//draw_vline_y(in, x, y, y+h, color);
	//draw_vline_y(in, x+w, y, y+h, color);
//}

static void draw_box(AVFrame *in, int x, int y, int w, int h){
	if(x<=0 || x+w >= in->width || y<=0 || y+h >= in->height ){
		return;
	}
	
	if(in->width/2 >= x && in->width/2 <= x+w && in->height/2 >= y && in->height/2 <= y+h){
		target=1;
	}else{
		target=0;
	}

	draw_pline(in, y, x, x+w);
	draw_pline(in, y+h, x, x+w);
	draw_vline(in, x, y, y+h);
	draw_vline(in, x+w, y, y+h);
}

//static void draw_cross_GREY8(AVFrame *in){
	//int lenth = in->height/20;
	//int s;
	//if(scale <= SCALE_TIME){
		//s = scale;
	//}else if(scale <= SCALE_TIME*2){
		//s = SCALE_TIME*2 - scale;
	//}else{
		//s = 0;
	//}
	//s=s*SCALE_RANGE;
	//
	//draw_pline_y(in, in->height/2, in->width/2 - lenth -s, in->width/2 - lenth + lenth/2 -s, 255);
	//draw_pline_y(in, in->height/2, in->width/2 + lenth/2 + s, in->width/2 + lenth + s, 255);
	//draw_vline_y(in, in->width/2, in->height/2 - lenth -s, in->height/2 - lenth + lenth/2 -s, 255);
	//draw_vline_y(in, in->width/2, in->height/2 + lenth/2 + s, in->height/2 + lenth +s, 255);
	//scale++;
//}

static void draw_cross(AVFrame *in){
	int lenth = in->height/20;
	int s;
	if(scale <= SCALE_TIME){
		s = scale;
	}else if(scale <= SCALE_TIME*2){
		s = SCALE_TIME*2 - scale;
	}else{
		s = 0;
	}
	s=s*SCALE_RANGE;
	
	draw_pline(in, in->height/2, in->width/2 - lenth -s, in->width/2 - lenth + lenth/2 -s);
	draw_pline(in, in->height/2, in->width/2 + lenth/2 + s, in->width/2 + lenth + s);
	draw_vline(in, in->width/2, in->height/2 - lenth -s, in->height/2 - lenth + lenth/2 -s);
	draw_vline(in, in->width/2, in->height/2 + lenth/2 + s, in->height/2 + lenth +s);
	scale++;
}

static int adjust_box(int * box){
	box[0] = box[0]*w/DCT_W;
	box[1] = box[1]*h/DCT_H;
	box[2] = box[2]*w/DCT_W;
	box[3] = box[2];
	return 0;	
}

static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
	AVFilterContext *ctx = inlink->dst;
	FoobarContext *foobar = ctx->priv;
	AVFilterLink *outlink = ctx->outputs[0];
	int bbox[4];
	double mret = 0;

	if(!zfq_flag){
		av_log(NULL, AV_LOG_INFO, "zffqueue init linesize %d, height %d\n", in->linesize[0], in->height);
		zffqueue_init(in->linesize[0] * in->height, 100, in->linesize[0]);
		zfq_flag = 1;
	}

	av_frame_make_writable(in);
	draw_cross(in);

	num++;
	if(foobar->mode){
		if(num >= NUM_XY && zswitch2){//end track
			if(target){
				scale = 0;
			}
			zswitch2 = 0;
			do_submit();
		}else if(!zswitch2 && (num%120) == 0){//begin to track
			scale = 0;
			zswitch2 = 1;
			num = 0;
			vnum = 0;
		}
	}

	if(zswitch2){//TODO
		//rgb = in->data[0];
		int outlinesize[4] = {DCT_W, 0, 0, 0};
		sws_scale(swsc, in->data, in->linesize, 0, h, &dtcbuf, outlinesize);
		rgb = dtcbuf;

		if(matcher == NULL){
			/*
			matcher = createMatcher(in->height, in->width, 0);
			bbox[0] = in->width/2-d/2;
			bbox[1] = in->height/2-d/2;
			bbox[2] = d;
			bbox[3] = d;
			*/
			matcher = createMatcher(DCT_H, DCT_W, 0);
			bbox[0] = DCT_W/2-d/2;
			bbox[1] = DCT_H/2-d/2;
			bbox[2] = d;
			bbox[3] = d;
			initMatcher(rgb, bbox, matcher);
			//av_log(NULL, AV_LOG_INFO, "here4\n");

		}else{
			bbox[0] = 0;
			bbox[1] = 0;
			bbox[2] = 0;
			bbox[3] = 0;
			mret = matchMatcher(rgb, bbox, matcher);
		}

		if(mret >= 0.8){
			adjust_box(bbox);
			draw_box(in, bbox[0], bbox[1], bbox[2], bbox[3]);
			if(vnum < NUM_XY){
				x[vnum] = bbox[0] + d/2;
				y[vnum] = bbox[1] + d/2;
				vnum++;
			}
		}
	}else{// see if there is any previous resource to be freed

		if(matcher){
			deleteMatcher(matcher);
			matcher = NULL;
		}
	}	


	zffqueue_put(in->data[0]);
	return ff_filter_frame(outlink, in);
}

static av_cold void uninit(AVFilterContext *ctx)
{
    
}


static int config_props(AVFilterLink *inlink)
{
	pxfmt = inlink->format; 
	h = inlink->h;
	w = inlink->w;
	
	//init sws scale context
	swsc = sws_getContext(w, h, AV_PIX_FMT_GRAY8, DCT_W, DCT_H, AV_PIX_FMT_GRAY8, SWS_POINT, NULL, NULL, NULL);
	if(swsc == NULL){
		av_log(NULL, AV_LOG_INFO, "error sws context");
	}
	
	dtcbuf = malloc(DCT_W * DCT_H);

	d = DCT_W/5;
	av_log(NULL, AV_LOG_INFO, "pxfmt %d, w %d, h %d, dctw %d, dcth %d, d %d\n", pxfmt, w, h, DCT_W, DCT_H, d);
	return 0;
}


static const AVFilterPad foobar_inputs[] = {
    {
        .name         = "default",
        .type         = AVMEDIA_TYPE_VIDEO,
        .config_props = config_props,
        .filter_frame = filter_frame,
    },
    { NULL }
};

static const AVFilterPad foobar_outputs[] = {
    {
        .name = "default",
        .type = AVMEDIA_TYPE_VIDEO,
    },
    { NULL }
};

AVFilter ff_vf_foobar = {
    .name          = "foobar",
    .description   = NULL_IF_CONFIG_SMALL("Track object."),
    .priv_size     = sizeof(FoobarContext),
    .init          = init,
    .uninit        = uninit,
    .query_formats = query_formats,
    .priv_class    = &foobar_class,
    .inputs        = foobar_inputs,
    .outputs       = foobar_outputs,
};
