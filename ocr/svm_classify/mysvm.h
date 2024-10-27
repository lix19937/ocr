
#include "cv.h"    
#include "highgui.h"  
#include <ml.h>  
#include <opencv2/ml/ml.hpp>
#include <iostream>    
#include <fstream>    
#include <string>    
#include <vector>
#include<io.h>

using namespace cv;
using namespace std;

class mysvm : public CvSVM
{
public:
	float predict(const float* row_sample, int row_len, bool returnDFVal,double &sum_ret) const
	{
		assert(kernel);
		assert(row_sample);

		int var_count = get_var_count();
		assert(row_len == var_count);
		(void)row_len;

		int class_count = class_labels ? class_labels->cols :
			params.svm_type == ONE_CLASS ? 1 : 0;

		float result = 0;
		cv::AutoBuffer<float> _buffer(sv_total + (class_count + 1) * 2);
		float* buffer = _buffer;

		if (params.svm_type == EPS_SVR ||
			params.svm_type == NU_SVR ||
			params.svm_type == ONE_CLASS)
		{
			CvSVMDecisionFunc* df = (CvSVMDecisionFunc*)decision_func;
			int i, sv_count = df->sv_count;
			double sum = -df->rho;

			kernel->calc(sv_count, var_count, (const float**)sv, row_sample, buffer);
			for (i = 0; i < sv_count; i++)
				sum += buffer[i] * df->alpha[i];

			result = params.svm_type == ONE_CLASS ? (float)(sum > 0) : (float)sum;
		}
		else if (params.svm_type == C_SVC ||
			params.svm_type == NU_SVC)
		{
			CvSVMDecisionFunc* df = (CvSVMDecisionFunc*)decision_func;
			int* vote = (int*)(buffer + sv_total);
			int i, j, k;

			memset(vote, 0, class_count*sizeof(vote[0]));
			kernel->calc(sv_total, var_count, (const float**)sv, row_sample, buffer);
			double sum = 0.;

			for (i = 0; i < class_count; i++)
			{
				for (j = i + 1; j < class_count; j++, df++)
				{
					sum = -df->rho;
					int sv_count = df->sv_count;
					for (k = 0; k < sv_count; k++)
						sum += df->alpha[k] * buffer[df->sv_index[k]];

					vote[sum > 0 ? i : j]++;
				}
			}

			for (i = 1, k = 0; i < class_count; i++)
			{
				if (vote[i] > vote[k])
					k = i;
			}
			//2017/07/06 sum_ret可以视作为置信度值
			vector<double> sum_test;
			CvSVMDecisionFunc* df1 = (CvSVMDecisionFunc*)decision_func;
			for (i = 0; i < class_count; i++){
				for (j = i + 1; j < class_count; j++, df1++){
					if (i == k || j == k){
						sum = -df1->rho;
						int sv_count = df1->sv_count;
						for (int kk = 0; kk < sv_count; kk++)
							sum += df1->alpha[kk] * buffer[df1->sv_index[kk]];
						sum_test.push_back(abs(sum));
					}
				}
			}
			sort(sum_test.begin(), sum_test.end(), less<double>());
			sum_ret = sum_test[0];
			//

			result = returnDFVal && class_count == 2 ? (float)sum : (float)(class_labels->data.i[k]);
		}
		else
			CV_Error(CV_StsBadArg, "INTERNAL ERROR: Unknown SVM type, "
			"the SVM structure is probably corrupted");
		return result;
	}
};