__kernel void cl_img_gaussian_blur(__global const uchar* in,
                                   __global uchar* out,
                                   __global const uint* gbox,
                                    uint n,
                                    uint sum,
                                    uint w,
                                    uint h,
                                    uint stride)
{
    int i, j, offset;
    uint x_id, y_id, x_pix, y_pix, summR, summG, summB;

    y_id = get_global_id(0);
    x_id = get_global_id(1);
    y_pix = get_global_id(0) * 4 ;
    x_pix = get_global_id(1) * 4 ;

    offset = n/2;

    /* ignore border pixels
     */
    if (y_id - offset < 0 || y_id + offset > h || x_id - offset < 0 || x_id + offset > w) {
        out[y_pix*stride + x_pix] = in[y_pix*stride + x_pix];
        out[y_pix*stride + x_pix + 1] = in[y_pix*stride + x_pix + 1];
        out[y_pix*stride + x_pix + 2] = in[y_pix*stride + x_pix + 2];
        out[y_pix*stride + x_pix + 3] = in[y_pix*stride + x_pix + 3];
        return;
    }

    summR = 0;
    summG = 0;
    summB = 0;

    for (j = -offset; j <= offset; j++) {
        for (i = -offset; i <= offset; i++) {
            summB += in[(y_id + j)*stride*4 + x_pix + i*4 + 2]*gbox[(j + offset)*n + i + offset];
            summR += in[(y_id + j)*stride*4 + x_pix + i*4 + 0]*gbox[(j + offset)*n + i + offset];
            summG += in[(y_id + j)*stride*4 + x_pix + i*4 + 1]*gbox[(j + offset)*n + i + offset];
        }
    }

    out[y_pix*stride + x_pix + 0] = summR/sum;
    out[y_pix*stride + x_pix + 1] = summG/sum;
    out[y_pix*stride + x_pix + 2] = summB/sum;
    out[y_pix*stride + x_pix + 3] = 255;
}