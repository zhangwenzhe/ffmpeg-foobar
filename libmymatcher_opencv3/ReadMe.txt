

int test_add(int a, int b);
// return a+b
void * createMatcher(int rows, int cols, int type);
// int rows     : 帧高度
// int cols     : 帧宽度
// int type     : 0 单通道灰度图，1 三通道彩图
// return 类的对象指针
void test_picture(int * frame, void * p);
// uint8_t * frame  : 当前帧
// void *p      : 类的对象指针
// 保存当前帧为test.jpg
void deleteMatcher(void * p);
// void * p     : 类的对象指针
void initMatcher(int * frame, double * bbox, void * p);
// uint8_t * frame  : 当前帧，一维数组，对于彩图是 b0 g0 r0 | b1 g1 r1 | b2 g2 r2 | ...；对于灰度图是 g0 g1 g2 ...
// double * bbox: 目标框坐标[x,y,width,height]，(x,y)为框的左上角坐标
// void *p      : 类的对象指针
double matchMatcher(int * frame, double * bbox, void * p);
// uint8_t * frame  : 当前帧，一维数组
// double * bbox: 预测坐标[x,y,width,height]
// void *p      : 类的对象指针
// return 置信度(>0.75表示匹配)


/* example
#include "mymatcher.h"
int main(){
	int x=test_add(1,2);                                // 测试
	uint8_t * frame = new uint8_t [rows*cols*channels]; // 分配存储帧的数组空间，帧大小为rows*cols，通道数为channels
	int * bbox = new int [4];                           // 分配目标框/预测框数组空间，[x,y,width,height]
	void * p = createMatcher(rows,cols,1);              // 建立mymatcher对象
	bool flag = FALSE;                                  // 标记是否瞄准确认
	
	while(1){
		frame=读取帧;
		if(瞄准确认){
			// 指定目标框坐标
			bbox[0] = 2;
			bbox[1] = 5;
			bbox[2] = 20;
			bbox[3] = 15;
			test_picture(frame,p);
			initMatcher(frame, bbox);  // 使用目标框初始化mymatcher
			flag = TURE;
		}
		if(flag){ // 已瞄准确认
			matchMatcher(frame, bbox); // 获取预测框bbox
			...                        // 使用bbox
		}
		else{ // 没有瞄准确认
			...
		}
		if(waitKey(20)=='q')
			break;
	}
	delete [] frame;
	delete [] bbox;
	deleteMatcher(p);	
	return 0;
}
*/