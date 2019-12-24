### OS environment: fedora
### Implement with QT C++.

1. RGB Extraction & Transformation
    * The format of image is Bitmap.
    * Each pixel have 4 byte.
    * Each byte corresponds to B, G, R, and A respectively.

    1. Extract each R, G, B from pixel.
    ``` javascript    
    int format = 4;
    for(int i = 0; i < image.height(); i++)
    {
        imagebits_32 = image.scanLine(i);
        for(int j = 0; j < image.width(); j++)
        {
            BChannel[i*image.width() + j] = imagebits_32[ j * format + 0];
            GChannel[i*image.width() + j] = imagebits_32[ j * format + 1];
            RChannel[i*image.width() + j] = imagebits_32[ j * format + 2];
        }
    }
    ```
    2. Using the algorithm
    `gray = 0.299*R + 0.587*G + 0.114*B`

    Result:
    ![](https://i.imgur.com/FTZ5t9p.png)

2. Smooth Filter
    * Using 3*3 mask(mean filter and median filter), and run through each pixel,
        * mean filter: replacing each pixel with average of neighboring pixels
        * median filter: replacing each pixel with the median of neighboring pixels
    1. mean filter
    ``` javascript
    for(int k = -1; k < 2; k++)
        for(int u = -1; u < 2; u++)
            temp += corrupted[(i+k)*width + (j+u)];
    smooth[i*width + j] = temp/9;
    ```
    2. median filter
    ``` javascript
    for(int k = -1; k < 2; k++)
        for(int u = -1; u < 2; u++)
            mask.push_back(corrupted[(i+k)*width + (j+u)]);
    std::sort(mask.begin(), mask.end());
    smooth [ i*width+j ] = mask[5];
    ```
    Result:
![](https://i.imgur.com/2GEuFEb.png)

3. Histogram Equalization
    1. calculating occurrences of gray level(0-255) from image
    2. calculating cumulative distribution function(cdf) of gray level(0-255)
    3. using histogram equalization formula:
    ![](https://i.imgur.com/Oq0AVvj.png)
    M and N stand for width and height.
    L is number of gray level.
    ``` javascript
    histogram_equal = (int)round( (double)(cdf[pixel_val] - cdf_min)/(double)(total_pixel - cdf_min) * 255 );
    ```
    4. using QChart to plot histogram
    Result:
    ![](https://i.imgur.com/qWMQxCj.png)

4. user-definedthresholding
    * input a threhold between 0-255
    * setting pixel value = 255 where pixel value greater than threshold, vice versa.
![](https://i.imgur.com/NUUYjBB.png)

5. Sobel edge detection
    * Using 3*3 mask, and run through each pixel
    ``` c
    //calculate approximations of the derivatives – horizontal changes
        const int H_mask[3][3] = {
          {-1, 0, 1},
          {-2, 0, 2},
          {-1, 0, 1}
        };
        //calculate approximations of the derivatives – vertical changes
        const int V_mask[3][3] = {
            {-1,-2,-1},
            {0 , 0, 0},
            {1 , 2, 1}
        };
    ```
    * At each point in the image, the resulting gradient approximations can be combined to give the gradient magnitude
    ![](https://i.imgur.com/cUm2kfV.png)

    ![](https://i.imgur.com/UCfP81X.png)

6. Image rotation and stretching
    * scaling: using inverse mapping, scans the output pixel locations and, at each location (x′ , y′), computes the corresponding location in the input image using (x , y) = A^−1 (x′ , y′).
    ``` c++
    for(ny = 0; ny < new_height; ny++)
        for(nx = 0; nx < new_width; nx++)
        {
            y = (int)(ny/scale_height + 0.5);
            x = (int)(nx/scale_width + 0.5);
            processed[ny * new_width + nx] = origin[y * width + x];
        }
    ```
    * rotation:
        1. adjusting width and height.
        Computing four corner coordinate of input image after rotation to find max_x, min_x, max_y and min_y.
        Computing new width and new height by subtracting max and min.
        ``` c++
        new_width = (int)(max_x-min_x+0.5);
        new_height= (int)(max_y-min_y+0.5);
        ```
        2. rotating around center
        計算新圖原點和舊圖原點的平移距離，inverse mapping 新圖的座標旋轉完後減去平移距離才會對應正確的就圖座標。
        ``` c++
        // distance between new graph center and old graph center
        shiftX = (new_centerX * cosValue + new_centerY * sinValue) - centerX;
        shiftY = (-new_centerX * sinValue + new_centerY * cosValue) - centerY;

        for(ny = 0; ny < new_height; ny++)
            for(nx = 0; nx < new_width; nx++)
            {
                x = (int)(nx * cosValue + ny * sinValue + 0.5 - shiftX);
                y = (int)(-nx * sinValue + ny * cosValue + 0.5 - shiftY);
                if(y >= 0 && y < height && x >= 0 && x < width){
                    processed[ny * new_width + nx] = origin[y * width + x];
                }
                else
                    processed[ny * new_width + nx] = 0;
            }
        ```
    ![](https://i.imgur.com/YeEKzKB.png)

Conclusion:
1. 對圖片進行操作時，需要注意座標和陣列的關係。
2. 利用inverse mapping比forward mapping 還要簡單有效，不用做interpolation圖片中間也不會有缺值產生。
3. 圖片旋轉，做完 resize 要做 rotation 時，無論對哪個點做旋轉都一樣的，因最後一定會被平移，使得最左下角的座標為 (0,0)。
