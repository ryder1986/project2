/* Haar features calculation */

#include "_cv.h"
#include <stdio.h>
//#include "../inc/CalStageSum.h"
#if 1
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
//#include <std.h>
//#include <log.h>
//#include <clk.h>
//#include <mem.h>
//#include <tsk.h>
//#include <gbl.h>
CvPoint g_ptTemp1;
typedef int sumtype;
//typedef int64 sqsumtype;
sumtype* g_ppsumRectP[300][16];
CvMat  integralSum;
 
const int detect_win_size[10][2]={{36,45},{40,50},{48,60},{56,70},{64,80},{76,95},{92,115},{108,135},{128,160},{152,190}};
const int factor_array[10]={36,40,48,56,64,76,92,108,128,152};

#define CALC_SUM_(p0, p1, p2, p3, offset) \
	((p0)[offset] - (p1)[offset] - (p2)[offset] + (p3)[offset])
#if 0
inline int calc(sumtype* p[16], int _offset) 
{
	//printf("[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n",p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13],p[14]);
	int cval = CALC_SUM_( p[5], p[6], p[9], p[10], _offset );

	return (CALC_SUM_( p[0], p[1], p[4], p[5], _offset ) >= cval ? 128 : 0) |   // 0
		(CALC_SUM_( p[1], p[2], p[5], p[6], _offset ) >= cval ? 64 : 0) |    // 1
		(CALC_SUM_( p[2], p[3], p[6], p[7], _offset ) >= cval ? 32 : 0) |    // 2
		(CALC_SUM_( p[6], p[7], p[10], p[11], _offset ) >= cval ? 16 : 0) |  // 5
		(CALC_SUM_( p[10], p[11], p[14], p[15], _offset ) >= cval ? 8 : 0)|  // 8
		(CALC_SUM_( p[9], p[10], p[13], p[14], _offset ) >= cval ? 4 : 0)|   // 7
		(CALC_SUM_( p[8], p[9], p[12], p[13], _offset ) >= cval ? 2 : 0)|    // 6
		(CALC_SUM_( p[4], p[5], p[8], p[9], _offset ) >= cval ? 1 : 0);
}
#else
inline int calc(sumtype* p[16], int _offset) 
{
    //int iP56910;
    int iP0145;
    int iP1256;
    int iP2367;
    int iP671011;
    int iP10111415;
    int iP9101314;
    int iP891213;    
    int iP4589;
	//printf("[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n",p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13],p[14]);
	int cval; // = iP56910;

    cval = p[5][_offset] - p[6][_offset] - p[9][_offset] + p[10][_offset];
    iP0145 = p[0][_offset] - p[1][_offset] - p[4][_offset] + p[5][_offset];
    iP1256 = p[1][_offset] - p[2][_offset] - p[5][_offset] + p[6][_offset];    
    iP2367 = p[2][_offset] - p[3][_offset] - p[6][_offset] + p[7][_offset];
    iP671011 = p[6][_offset] - p[7][_offset] - p[10][_offset] + p[11][_offset];
    iP10111415 = p[10][_offset] - p[11][_offset] - p[14][_offset] + p[15][_offset];
    iP9101314 = p[9][_offset] - p[10][_offset] - p[13][_offset] + p[14][_offset];
    iP891213 = p[8][_offset] - p[9][_offset] - p[12][_offset] + p[13][_offset];
    iP4589 = p[4][_offset] - p[5][_offset] - p[8][_offset] + p[9][_offset];



    iP0145 = iP0145 >= cval ? 128 : 0;
    iP1256 = iP1256 >= cval ? 64 : 0;
    iP2367 = iP2367 >= cval ? 32 : 0;
    iP671011 = iP671011 >= cval ? 16 : 0;
    iP10111415 = iP10111415 >= cval ? 8 : 0;
    iP9101314 = iP9101314 >= cval ? 4 : 0;
    iP891213 = iP891213 >= cval ? 2 : 0;
    iP4589 = iP4589 >= cval ? 1 : 0;

	return iP0145 |   // 0
		iP1256 |    // 1
		iP2367 |    // 2
		iP671011 |  // 5
		iP10111415 |  // 8
		iP9101314 |   // 7
		iP891213 |    // 6
		iP4589;
}
#endif

#if 0
int cvRunLBPClassifierCascade1(CvLBPClassifierCascade* _cascade,int start_stage, int end_stage, int p_offset)
{
    int i = 0, j = 0;
    int iIW = 0;
    int ntrees;
	int nodeOfs = 0, leafOfs = 0;
	int subsetSize = 8 /*(_cascade->ncategories + 31)/32*/;
	int* cascadeSubsets =&(_cascade->subsets[0]);
	int* cascadeLeaves = &(_cascade->leaves[0]);
	Stage* cascadeStages = &(_cascade->stages[0]);
	sumtype** p;

    // 以下计算每个classifier的用到的特征区域的特征值
    for (i = start_stage; i < end_stage; i++)
    {
        long long stage_sum = 0;
        iIW = cascadeStages[i].first;
		ntrees = cascadeStages[i].ntrees;
		nodeOfs = iIW;
		leafOfs=iIW << 1;
        //leafOfs = iIW << 1;

        for (j = 0; j < ntrees; j++)
        //for (j = ntrees - 1; j >= 0; j--)
		{    
			int c;
			// 计算LBP特征
			//memcpy(p,g_ppsumRectP[nodeOfs],16 * sizeof(sumtype));
            //memcpy(p, g_ppsumRectP[nodeOfs], 64);
			p = g_ppsumRectP[nodeOfs];
            c = calc(p, p_offset);
			const int* subset = &cascadeSubsets[nodeOfs * subsetSize];
			stage_sum += cascadeLeaves[subset[c >> 5] & (1 << (c & 31)) ? leafOfs : leafOfs + 1];
			nodeOfs++;
			leafOfs += 2;
		}
        
		if (stage_sum < cascadeStages[i].threshold)
		{
			return i;
		}
	}
    
    return end_stage;
}
#else
int cvRunLBPClassifierCascade1(CvLBPClassifierCascade* _cascade,int start_stage, int end_stage, int p_offset)
{
    int i = 0, j = 0;
    int iIW = 0;
    int ntrees;
	int nodeOfs = 0, leafOfs = 0;
	//int subsetSize = 8 /*(_cascade->ncategories + 31)/32*/;
	int* cascadeSubsets = &(_cascade->subsets[0]);
	int* cascadeLeaves = &(_cascade->leaves[0]);
	Stage* cascadeStages = &(_cascade->stages[0]);
	sumtype** p;
	//sumtype* p[16];    
    
    //int iCond;

	//int * piTemp;	
    //volatile int *g_piP0;   
    // 以下计算每个classifier的用到的特征区域的特征值
    for (i = start_stage; i < end_stage; ++i)
    {
        long long stage_sum = 0;
        iIW = cascadeStages[i].first;
		ntrees = cascadeStages[i].ntrees;
		nodeOfs = iIW;
		//leafOfs = iIW * 2;
        leafOfs = iIW << 1;

        //for (j = 0; j < ntrees; j++)
        for (j = ntrees - 1; j >= 0; --j)
		{    
			 int c;
			// 计算LBP特征

			 //memcpy(p, g_ppsumRectP[nodeOfs], 16 * sizeof(sumtype));
			 //memcpy(p, g_ppsumRectP[nodeOfs], 64);
			 p = g_ppsumRectP[nodeOfs];
			 //int iP56910;
			 int iP0145;
			 int iP1256;
			 int iP2367;
			 int iP671011;
			 int iP10111415;
			 int iP9101314;
			 int iP891213;    
			 int iP4589;
			 int cval; // = iP56910;

			 iP0145 = p[0][p_offset] - p[1][p_offset] - p[4][p_offset] + p[5][p_offset];
			 iP1256 = p[1][p_offset] - p[2][p_offset] - p[5][p_offset] + p[6][p_offset];    
			 iP2367 = p[2][p_offset] - p[3][p_offset] - p[6][p_offset] + p[7][p_offset];
			 iP671011 = p[6][p_offset] - p[7][p_offset] - p[10][p_offset] + p[11][p_offset];
			 iP10111415 = p[10][p_offset] - p[11][p_offset] - p[14][p_offset] + p[15][p_offset];
			 iP9101314 = p[9][p_offset] - p[10][p_offset] - p[13][p_offset] + p[14][p_offset];
			 iP891213 = p[8][p_offset] - p[9][p_offset] - p[12][p_offset] + p[13][p_offset];
			 iP4589 = p[4][p_offset] - p[5][p_offset] - p[8][p_offset] + p[9][p_offset];

			 cval = p[5][p_offset] - p[6][p_offset] - p[9][p_offset] + p[10][p_offset];

			 iP0145 = iP0145 >= cval ? 128 : 0;
			 iP1256 = iP1256 >= cval ? 64 : 0;
			 iP2367 = iP2367 >= cval ? 32 : 0;
			 iP671011 = iP671011 >= cval ? 16 : 0;
			 iP10111415 = iP10111415 >= cval ? 8 : 0;
			 iP9101314 = iP9101314 >= cval ? 4 : 0;
			 iP891213 = iP891213 >= cval ? 2 : 0;
			 iP4589 = iP4589 >= cval ? 1 : 0;

			 c = iP0145 |   // 0
				 iP1256 |    // 1
				 iP2367 |    // 2
				 iP671011 |  // 5
				 iP10111415 |  // 8
				 iP9101314 |   // 7
				 iP891213 |    // 6
				 iP4589;


			 //const int* subset = &cascadeSubsets[nodeOfs * subsetSize];
			 const int* subset = &cascadeSubsets[nodeOfs << 3];         
			 stage_sum += cascadeLeaves[subset[c >> 5] & (1 << (c & 31)) ? leafOfs : leafOfs + 1];

			 nodeOfs++;
			 leafOfs += 2;
           
		}
        
		if (stage_sum < cascadeStages[i].threshold)
		{
			return i;
		}
	}
    
    return end_stage;
}
#endif

 int  
cvRunLBPClassifierCascade0(CvLBPClassifierCascade* _cascade,CvSize win_size)
{
    int p_offset;
	//int i=0,j=0;
    
    // 确保矩形的有效性，并防止计算窗口出边界
    if (g_ptTemp1.x < 0 || g_ptTemp1.y < 0 
     || g_ptTemp1.x + win_size.width >= integralSum.width - 2 
     || g_ptTemp1.y + win_size.height >= integralSum.height - 2)
    {
        return -1;
    }
    
    // 计算特征点在积分图中的偏移，相当于移动窗口
    p_offset = g_ptTemp1.y * (integralSum.step >> 2) + g_ptTemp1.x;

    // 用检测窗口扫描两遍图像
	// 第一遍通过级联两个stage粗略定位目标大致区域，对候选区域进行标定
    //int split_stage = 2; //第一次分类玫降淖畲髎tage，第二次分类用到的起始stage
    int ret = 0; // 返回的遍历层数
    /*ret = cvRunLBPClassifierCascade1(_cascade, 0,split_stage, p_offset);

    if (ret < split_stage)
    {
    	return ret;
    }*/

    //第二遍对标定的候选区域进行完整筛选，将候选区域放置到队列中
    ret = cvRunLBPClassifierCascade1(_cascade,0, _cascade->stage_count, p_offset);

    return ret;
}
CV_IMPL void
cvSetImagesForLBPClassifierCascade( CvLBPClassifierCascade* _cascade,
                                     const CvArr* _sum,
                                    int scale32x)
{
    //CV_FUNCNAME("cvSetImagesForLBPClassifierCascade");

   // __BEGIN__;

    CvMat sum_stub, *sum = (CvMat*)_sum;
    int coi0 = 0;
    int i;
    sumtype *piSumTemp;
    
    unsigned int nSumStep;
    unsigned int nSumStepDiv4;
    sum = cvGetMat(sum, &sum_stub, &coi0);
    
    piSumTemp = (sumtype *)((*sum).data.ptr);
    
    nSumStep = (*sum).step;
    nSumStepDiv4 = nSumStep >> 2;
    
    /* init pointers in haar features according to real window size and
       given image pointers */
	DTreeNode*  node=_cascade->nodes;
	Feature* feature=_cascade->features;
	for (i = 0; i < _cascade->node_count; ++i)
	{
		int k;
		CvRect tr,r;
		k=node[i].featureIdx;
		r.x=feature[k].x;
		r.y=feature[k].y;
		r.width=feature[k].width;
		r.height=feature[k].height;
		tr.x = ( r.x * scale32x +16)>>5;
		tr.width = ( r.width * scale32x+16 )>>5;
		tr.y = ( r.y * scale32x + 16 )>>5;
		tr.height = ( r.height * scale32x +16 )>>5;
	    //r1=tr;
	    g_ppsumRectP[i][0] = piSumTemp + nSumStepDiv4 * tr.y + tr.x;
		g_ppsumRectP[i][1] = piSumTemp + nSumStepDiv4 * tr.y + tr.x + tr.width;
		g_ppsumRectP[i][4] = piSumTemp + nSumStepDiv4 * (tr.y + tr.height) + tr.x;
		g_ppsumRectP[i][5] = piSumTemp + nSumStepDiv4 * (tr.y + tr.height) + tr.x + tr.width;
		tr.x += (tr.width << 1);
		g_ppsumRectP[i][2] = piSumTemp + nSumStepDiv4 * tr.y + tr.x;
		g_ppsumRectP[i][3] = piSumTemp + nSumStepDiv4 * tr.y + tr.x + tr.width;
		g_ppsumRectP[i][6] = piSumTemp + nSumStepDiv4 * (tr.y + tr.height) + tr.x;
		g_ppsumRectP[i][7] = piSumTemp + nSumStepDiv4 * (tr.y + tr.height) + tr.x + tr.width;
		tr.y += (tr.height << 1);
		g_ppsumRectP[i][10] = piSumTemp + nSumStepDiv4 * tr.y + tr.x;
		g_ppsumRectP[i][11] = piSumTemp + nSumStepDiv4 * tr.y + tr.x + tr.width;
		g_ppsumRectP[i][14] = piSumTemp + nSumStepDiv4 * (tr.y + tr.height) + tr.x;
		g_ppsumRectP[i][15] = piSumTemp + nSumStepDiv4 * (tr.y + tr.height) + tr.x + tr.width;
		tr.x -= (tr.width << 1);
		g_ppsumRectP[i][8] = piSumTemp + nSumStepDiv4 * tr.y + tr.x;
		g_ppsumRectP[i][9] = piSumTemp + nSumStepDiv4 * tr.y + tr.x + tr.width;
		g_ppsumRectP[i][12] = piSumTemp + nSumStepDiv4 * (tr.y + tr.height) + tr.x;
		g_ppsumRectP[i][13] = piSumTemp + nSumStepDiv4 * (tr.y + tr.height) + tr.x + tr.width;

	}
    //__END__;
}
static int is_equal( const void* _r1, const void* _r2, void* )
{
	const CvRect* r1 = (const CvRect*)_r1;
	const CvRect* r2 = (const CvRect*)_r2;
	int distance5x = r1->width ;//int distance = cvRound(r1->width*0.2);

	return r2->x*5 <= r1->x*5 + distance5x &&
		r2->x*5 >= r1->x*5 - distance5x &&
		r2->y*5 <= r1->y*5 + distance5x &&
		r2->y*5 >= r1->y*5 - distance5x &&
		r2->width*5 <= r1->width * 6 &&
		r2->width * 6 >= r1->width*5;
}
CV_IMPL CvSeq*
	detectMultiScale( const CvArr* _img, unsigned char* maskImg,
                     CvLBPClassifierCascade* cascade,
                     CvMemStorage* storage, int scale_factor32x,
                     int min_neighbors, int flags, CvSize min_size, CvSize max_size)
{
    //int split_stage = 2;

    CvMat stub, *img = (CvMat*)_img;
    CvMat *sum = 0/*, *tilted = 0, *sqsum = 0, *norm_img = 0, *img_small = 0*/;
    CvSeq* seq = 0;
    CvSeq* seq2 = 0;
    CvSeq* idx_seq = 0;
    CvSeq* result_seq = 0;
    CvMemStorage* temp_storage = 0;
    CvAvgComp* comps = 0;
    int i;
	//const CvRect rect_array[10]={{1,1,30,38},{1,1,36,45},{1,1,43,55},{2,2,52,65},{2,2,62,78},{3,3,75,95},{3,3,90,114},{4,4,108,137},{(factor_array[8]+16)>>5,(factor_array[8]+16)>>5,((32-2)*factor_array[8] + 16 ) >> 5,((40-2)*factor_array[8] + 16 ) >> 5},{(factor_array[9]+16)>>5,(factor_array[9]+16)>>5,((32-2)*factor_array[9] + 16 ) >> 5,((40-2)*factor_array[9] + 16 ) >> 5}};
    int result;
    CvRect rectTemp; 
    //CV_FUNCNAME("cvHaarDetectObjects");

    //__BEGIN__;

    int factor32x;
    int /*npass = 2,*/ coi;
    int cols;

    //CV_CALL(img = cvGetMat( img, &stub, &coi));
    img = cvGetMat(img, &stub, &coi);  	
	cols = img->cols;

    //CV_CALL( sum = cvCreateMat( img->rows + 1, img->cols + 1, CV_32SC1 ));
    //CV_CALL( sqsum = cvCreateMat( img->rows + 1, img->cols + 1, CV_64FC1 ));
    //CV_CALL( temp_storage = cvCreateChildMemStorage( storage ));



    sum = cvCreateMat(img->rows + 1, img->cols + 1, CV_32SC1);
    //sqsum = cvCreateMat(img->rows + 1, img->cols + 1, CV_64FC1);
    temp_storage = cvCreateChildMemStorage(storage);

    seq = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvRect), temp_storage );
    seq2 = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvAvgComp), temp_storage );
    result_seq = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvAvgComp), storage );
    //if( min_neighbors == 0 )
    //{
    //    seq = result_seq;
    //}
	
	cvIntegral( img, sum, 0, 0 );
    
    integralSum=*sum;
    //if ((unsigned)split_stage >= (unsigned)cascade->stage_count)
    //{
    //    split_stage = cascade->stage_count;
    //    npass = 1;
    //}

	for( /*factor32x = 32; factor32x*cascade->orig_window_size.width < (img->cols - 10)<<5 &&
		 factor32x*cascade->orig_window_size.height < (img->rows - 10)<<5;
		 factor32x = (factor32x*scale_factor32x+16)>>5*/i = 0; i < 10; ++i)
	{
		factor32x=factor_array[i];
		//const int ystep32x =MAX(64, factor32x );
		int ystep32x = ( factor32x >= 64 )? 32 : 64;
		//int _xstep = ( ystep32x < 50 ) ? 2 : 1;
		//int  _xstep = 1;
		//const int ystep32x = MAX( 64, detect_win_size[i][0] );
		/*CvSize win_size = { ( cascade->orig_window_size.width * factor32x + 16 )>>5,
		( cascade->orig_window_size.height * factor32x + 16)>>5 };*/
		CvSize win_size={detect_win_size[i][0],detect_win_size[i][1]};
		if(ystep32x == 32)
			ystep32x = ystep32x * win_size.width /32;//40为分类器检测窗口宽度
		//CvRect equ_rect = { 0, 0, 0, 0 };
		//int *p0 = 0, *p1 = 0, *p2 = 0, *p3 = 0;
		//int *pq0 = 0, *pq1 = 0, *pq2 = 0, *pq3 = 0;
		//int pass, stage_offset = 0;

		int stop_height = (((img->rows - win_size.height) << 5) + (ystep32x >> 1)) / ystep32x;
		int stop_width =(((img->cols - win_size.width) << 5) + (ystep32x >> 1)) / ystep32x;
		if( win_size.width < min_size.width || win_size.height < min_size.height )
		{
			continue;
		}
		if(win_size.width>max_size.width||win_size.height>max_size.height)
		{
			break;
		}
		if( win_size.width > img->cols-10 || win_size.height>img->rows-10 )
		{
			break;
		}
		cvSetImagesForLBPClassifierCascade( cascade, sum, factor32x);

		if(maskImg)
		{
			for( int _iy = 0; _iy < stop_height; _iy++)
			{
				int iy = (_iy*ystep32x+16)>>5; // 检测窗口纵坐标步长为2
				int _ix,_xstep = 1, preStep = 1;
				for( _ix = 0; _ix < stop_width; _ix+=_xstep )
				{
					int ix = (_ix*ystep32x+16)>>5; // it really should be ystep
					if(*(maskImg+cols*iy+ix)==0)
						continue;
					//_xstep = 2; // 检测窗口横坐标按步长为4开始移动
					_xstep = 1 + (preStep < 2); // 检测窗口横坐标按步长为4开始移动
					g_ptTemp1.x = ix;
					g_ptTemp1.y = iy;
					result = cvRunLBPClassifierCascade0(cascade, win_size);
					if (result == 0)
					{
						_ix++; // 未通过第一个分类器，将步长设为2
					}
					else if ( result >= cascade->stage_count ) // 通过所有分类器，表示找到目标
					{
						rectTemp = cvRect(ix, iy, win_size.width, win_size.height);
						cvSeqPush(seq, &rectTemp);
					}
					preStep = _xstep;
				}
			}
		}
		else
		{
			for ( int _iy = 0; _iy < stop_height; _iy++)
			{
				int iy = (_iy * ystep32x + 16) >> 5; // 检测窗口纵坐标步长为2
				int _ix,_xstep = 1, preStep = 1;
				for ( _ix = 0; _ix < stop_width; _ix+=_xstep)
				{
					int ix = (_ix * ystep32x + 16) >> 5; // it really should be ystep

					//result = cvRunHaarClassifierCascade(cascade, cvPoint(ix,iy), 0);
					_xstep = 1 + (preStep < 2); // 检测窗口横坐标按步长为4开始移动
					g_ptTemp1.x = ix;
					g_ptTemp1.y = iy;
					result = cvRunLBPClassifierCascade0(cascade, win_size);
					if (result == 0)
					{
						_ix++; // 未通过第一个分类器，将步长设为2
					}
					else if ( result >= cascade->stage_count ) // 通过所有分类器，表示找到目标
					{
						rectTemp = cvRect(ix, iy, win_size.width, win_size.height);

						cvSeqPush(seq, &rectTemp);
					}
					preStep = _xstep;
				}
			}

		}
					//_xstep = 2; // 检测窗口横坐标按步长为4开始移动

					//result = cvRunHaarClassifierCascade(cascade, cvPoint(ix,iy), 0);


	}
   

    if( min_neighbors != 0 )
    {
        // group retrieved rectangles in order to filter out noise 
        int ncomp = cvSeqPartition( seq, 0, &idx_seq, (CvCmpFunc)is_equal, 0 );
        comps = (CvAvgComp*)cvAlloc((ncomp + 1) * sizeof(comps[0]));
        memset( comps, 0, (ncomp+1)*sizeof(comps[0]));
        // count number of neighbors
        for ( i = 0; i < seq->total; ++i )
        {
            CvRect r1 = *(CvRect*)cvGetSeqElem( seq, i );
            int idx = *(int*)cvGetSeqElem( idx_seq, i );
            assert( (unsigned)idx < (unsigned)ncomp );

            comps[idx].neighbors++;
             
            comps[idx].rect.x += r1.x;
            comps[idx].rect.y += r1.y;
            comps[idx].rect.width += r1.width;
            comps[idx].rect.height += r1.height;
        }

        // calculate average bounding box
        for ( i = 0; i < ncomp; ++i )
        {
            int n = comps[i].neighbors;
			int N2 = n << 1;
            if( n >= min_neighbors )
            {
                CvAvgComp comp;
                comp.rect.x = (( comps[i].rect.x << 1) + n) / N2;
                comp.rect.y = (( comps[i].rect.y << 1) + n) / N2;
                comp.rect.width = ((comps[i].rect.width << 1) + n) / N2;
                comp.rect.height = ((comps[i].rect.height << 1) + n) / N2;
                comp.neighbors = comps[i].neighbors;

                cvSeqPush( seq2, &comp );
            }
        }

        // filter out small face rectangles inside large face rectangles
        for ( i = 0; i < seq2->total; ++i )
        {
            CvAvgComp r1 = *(CvAvgComp*)cvGetSeqElem( seq2, i );
            int j, flag = 1;

            for ( j = 0; j < seq2->total; ++j )
            {
                CvAvgComp r2 = *(CvAvgComp*)cvGetSeqElem( seq2, j );
                int distance = ((r2.rect.width << 1)  + 5) / 10;//cvRound( r2.rect.width * 0.2 );
            
                if( i != j &&
                    r1.rect.x >= r2.rect.x - distance &&
                    r1.rect.y >= r2.rect.y - distance &&
                    r1.rect.x + r1.rect.width <= r2.rect.x + r2.rect.width + distance &&
                    r1.rect.y + r1.rect.height <= r2.rect.y + r2.rect.height + distance &&
                    (r2.neighbors > MAX( 3, r1.neighbors ) || r1.neighbors < 3) )
                {
                    flag = 0;
                    break;
                }
            }

            if( flag )
            {
                cvSeqPush( result_seq, &r1 );
                 //cvSeqPush( result_seq, &r1.rect ); 
            }
        }
    }

    //__END__;




//printf("------------------\n");
    cvReleaseMemStorage( &temp_storage );
    cvReleaseMat( &sum );
   // cvReleaseMat( &norm_img );
   // cvReleaseMat( &img_small );
    cvFree( &comps );


    return result_seq;
}


#endif
