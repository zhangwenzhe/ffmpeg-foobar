

/**
 * @file
 * object track filter
 *
 */
#include <signal.h>
#include "libavutil/avassert.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
#include "avfilter.h"
#include "formats.h"
#include "internal.h"
#include "video.h"
#include <mymatcher.h>


#define SCALE_TIME 7
#define SCALE_RANGE 3


#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))



static int zswitch2=0;
static uint8_t *rgb = NULL;
//static uint8_t *yuv420p = NULL;
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

/*
typedef struct FoobarContext {
	int autotrigger;
    const AVClass *class;
} FoobarContext;

#define OFFSET(x) offsetof(FoobarContext, x)

static const AVOption foobar_options[] = {
    { "autotrigger", "autotrigger?",  OFFSET(autotrigger), AV_OPT_TYPE_INT, {.i64=0}, 0, 1, AV_OPT_FLAG_VIDEO_PARAM, NULL },
    { NULL }
};
*/

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

	//av_log(NULL, AV_LOG_INFO, "draw_box x %d, y %d, w %d, h %d, rx %d, ry %d\n", 
		//y, x, w, h, x+w, y+h);
	
	//draw_pline_y(in, y, x, x+w, 255);
	//draw_pline_y(in, y+h, x, x+w, 255);
	
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

static void make_grey(AVFrame *in){
	int zi;
	if(in->format == 0){
		for(zi=0; zi < in->width * in->height / 4; zi++){
			if(in->data[1][zi] != 0 && in->data[2][zi] != 0){
				in->data[1][zi] = 128;
				in->data[2][zi] = 128;
			}
		}
		
		
	}/*else if(in->format == 1){
		for(zi=0; zi < in->width * in->height * 2; zi++){
			if(zi%2 == 1){
				in->data[0][zi] = 128;
			}

		}
	}
	*/
}

static inline void _yuv2rgb(uint8_t *out, int ridx, int Y, int U, int V)
{
    out[ridx]     = av_clip_uint8(Y +              (91881 * V + 32768 >> 16));
    out[1]        = av_clip_uint8(Y + (-22554 * U - 46802 * V + 32768 >> 16));
    out[2 - ridx] = av_clip_uint8(Y + (116130 * U             + 32768 >> 16));
}

static inline uint8_t bound(uint8_t a, uint8_t b, uint8_t c){
	if(b > c){
		return c;
	}else if(b < a){
		return a;
	}else{
		return b;
	}
}

static inline void _yuv2rgb2(uint8_t *out, int ridx, int Y, int U, int V)
{
    uint8_t R;
    uint8_t G;
    uint8_t B;

	R = (int)((Y - 16) + (1.370705 * (V - 128)));
	G = (int)((Y - 16) - (0.698001 * (V - 128)) - (0.337633 * (U - 128)));
	B = (int)((Y - 16) + (1.732446 * (U - 128)));

	R = bound(0, R, 255);
	G = bound(0, G, 255);
	B = bound(0, B, 255);

	out[ridx]     = R;
    out[1]        = G;
    out[2 - ridx] = B;
}


static int yuv2rgb(AVFrame *in, uint8_t *ret){
	if(in->format == 0){
		int i;
		for(i=0; i<in->width * in->height; i++){
			_yuv2rgb2(ret+i*3, 2, in->data[0][i], in->data[1][i/4], in->data[2][i/4]);
		}
		return 0;
	}
	return 1;
}

static int yuv2grey(AVFrame *in, uint8_t *ret){
	if(in->format == 0){
		memcpy(ret, in->data[0], in->width * in->height);
		return 0;
	}
	return 1;
}

static int yuv2yuv(AVFrame *in, uint8_t *ret){
	memcpy(ret, in->data[0], in->width * in->height);
	memcpy(ret + in->width * in->height, in->data[1], in->width * in->height/2);
	memcpy(ret + in->width * in->height/2, in->data[2], in->width * in->height/2);
	return 1;
}



static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    FoobarContext *foobar = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
	int bbox[4];
	double mret = 0;

	
    //AVFrame *out;
	//int inplace = 0;

	/*
	if(!zswitch2){	
		return ff_filter_frame(outlink, in);//zwz
	}
	*/

	/*
	if (av_frame_is_writable(in)) {
		inplace = 1;
        out = in;
    } else {
        out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(out, in);
    }
    */

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

	//av_log(NULL, AV_LOG_INFO, "format is %d, width %d, height %d, %d, %d\n", 
		//in->format, in->width, in->height, 
		//in->linesize[0], in->linesize[1]);//zwz

	if(zswitch2){//TODO
		/*
		if(yuv420p == NULL){
			yuv420p = av_malloc(in->width * in->height + in->width * in->height/2);
		}
		*/
		/*
		if(rgb == NULL){
			rgb = av_malloc(in->width * in->height * 3);
		}
		*/
		
		
		
		/*set rgb*/
		//make_grey(in);
		//av_log(NULL, AV_LOG_INFO, "here1\n");
		//yuv2rgb(in, rgb);
		//yuv2grey(in, rgb);
		rgb = in->data[0];
		//av_log(NULL, AV_LOG_INFO, "here2\n");
		//yuv2yuv(in, yuv420p);
		

		if(matcher == NULL){
			//av_log(NULL, AV_LOG_INFO, "here1\n");
			matcher = createMatcher(in->height, in->width, 0);
			//test_picture(yuv420p, matcher);
			//av_log(NULL, AV_LOG_INFO, "here2\n");
			
			bbox[0] = in->width/2-d/2;
			bbox[1] = in->height/2-d/2;
			bbox[2] = d;
			bbox[3] = d;
			//av_log(NULL, AV_LOG_INFO, "here3\n");
			initMatcher(rgb, bbox, matcher);
			//av_log(NULL, AV_LOG_INFO, "here4\n");
			
		}else{
			bbox[0] = 0;
			bbox[1] = 0;
			bbox[2] = 0;
			bbox[3] = 0;
			//av_log(NULL, AV_LOG_INFO, "here5\n");
			mret = matchMatcher(rgb, bbox, matcher);
			//av_log(NULL, AV_LOG_INFO, "here6\n");
		}

		//av_log(NULL, AV_LOG_INFO, "box is %d, %d, %d, %d\n", 
			//bbox[0], bbox[1], bbox[2], bbox[3]);

		
		if(mret >= 0.8){
			draw_box(in, bbox[0], bbox[1], bbox[2], bbox[3]);
			if(vnum < NUM_XY){
				x[vnum] = bbox[0] + 60;
				y[vnum] = bbox[1] + 60;
				vnum++;
			}
		}
		

	}else{// see if there is any previous resource to be freed
		
		if(matcher){
			//av_log(NULL, AV_LOG_INFO, "here7\n");
			deleteMatcher(matcher);
			//av_log(NULL, AV_LOG_INFO, "here8\n");
			matcher = NULL;
		}

		/*
		if(yuv420p){
			av_free(yuv420p);
			yuv420p = NULL;
		}
		*/

		/*
		if(rgb){
			av_free(rgb);
			rgb = NULL;
		}
		*/
		
	}	
	
    
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

	d = w/5;
	av_log(NULL, AV_LOG_INFO, "pxfmt %d, w %d, h %d\n", pxfmt, w, h);
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
