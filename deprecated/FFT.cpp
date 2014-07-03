//solving Poisson Equation with FFT
void fft_solve(Mat* Fields, Mat& Nf, Scalar mean_orig, Mat& channels, vector<Mat>& result)
{
    //the input Fields need to be remapped with Nf on each channel
    //the result is saved to result

    DataType * p_row;

    const int M = Fields[0].rows;
    const int N = Fields[0].cols;
    const int Total = (M-2)*(N-2);
    const DataType normalization = 1.0/(4*M*N);

    //fill the dominator for DST
    DataType* The_D = new DataType[Total];

    double tmp;
    p_row = The_D;
    const double tmp_r = PI/(2*M-2);
    const double tmp_c = PI/(2*N-2);
    double tmp_d;

    for (int i = 1; i < M-1; ++i)
    {
        tmp_d = sin(i*tmp_r);
        tmp = tmp_d * tmp_d;
        for (int j = 1; j < N-1; ++j)
        {
            tmp_d = sin(j*tmp_c);

            *p_row++ = 1.0/(4*(tmp + tmp_d*tmp_d)) ;
        }
    }

    //****************do the work here *************//
    DataType* MappedField = new DataType[Total];
    DataType* OrigField = new DataType[Total];

    DataType * p_Orig;
    DataType * p_MF; 

    fftwf_plan dct_fw, dct_bw;

    DataType* data_dct = new DataType[Total];

    Vec3b * p_result;

    for (int i = 0; i < 3; ++i)
    {
        //copy to orig field
        p_Orig = OrigField;
        for (int k = 1; k < M-1; ++k)
        {
            p_row = Fields[i].ptr<DataType>(k);
            for (int s = 1; s < N-1; ++s)
            {
                *p_Orig++ = p_row[s];
            }

        }

        //process this channel
        for (int j = 0; j < Nf.rows; ++j)
        {
            //map the field
            p_MF = MappedField;
            p_Orig = OrigField;
            DataType scale = Nf.at<DataType>(j,i);
            for (int s = 0; s < Total; ++s)
            {
                (*p_MF++) = -(*p_Orig++)*scale;
            }

            //DST(DCT)
            dct_fw = fftwf_plan_r2r_2d(M-2, N-2,MappedField, data_dct,FFTW_RODFT00, FFTW_RODFT00,FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
            //dct_fw = fftwf_plan_r2r_2d(M-2, N-2,MappedField, data_dct,FFTW_REDFT10, FFTW_REDFT10,FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
            fftwf_execute(dct_fw);

            //divide
            p_row = The_D;
            p_MF = data_dct;
            for (int s = 0; s < Total; ++s)
            {
                *p_MF++ *= (*p_row++);
            }

            // run the reverse, MappedField (reused) //
            dct_bw = fftwf_plan_r2r_2d(M-2, N-2,data_dct, MappedField,FFTW_RODFT00, FFTW_RODFT00,FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
            //dct_bw = fftwf_plan_r2r_2d(M-2, N-2,data_dct, MappedField,FFTW_REDFT01, FFTW_REDFT01,FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
            fftwf_execute(dct_bw);


            //normalization because of fftw 
            p_MF = MappedField;
            for (int s = 0; s < Total; ++s)
            {
                *p_MF++ *= normalization;
            }

            //shift such that the mean of result and original are the same
            DataType mean_result = 0;
            p_MF = MappedField;
            for (int s = 0; s < Total; ++s)
            {
                mean_result += (*p_MF++);
            }
            mean_result /= Total;

            //the 0.5 is for the round into unsigned char
            DataType mean_offset = mean_orig.val[i] - mean_result + 0.5;

            //put it into result_one_channel
            p_row = MappedField + (Pad_SizeR-1)*(N-2)+Pad_SizeC-1;

            for (int k = 0; k < channels.rows; ++k)
            {
                p_result = result[j].ptr<Vec3b>(k);
                for (int q = 0; q < channels.cols; ++q)
                {
                    tmp = (*p_row++) + mean_offset;
                    if(tmp<=0) 
                    {
                        p_result[q][i] = 0;
                    }else if (tmp>=255)
                    {
                        p_result[q][i] = 255;
                    }else
                    {
                        p_result[q][i] = (unsigned char)(tmp);
                    }
                }
                p_row += (2*Pad_SizeC-2);
            } 

        }  
    }

    /* cleanup */
    fftwf_destroy_plan(dct_fw);
    fftwf_destroy_plan(dct_bw);
    fftwf_cleanup();

    delete [] The_D;
    delete [] MappedField;
    delete [] OrigField;
    delete [] data_dct;
}
