# -*- encoding: utf-8 -*-
'''
@File    :   noiser.py
@Contact :   913799761@qq.com
@License :   (C)Copyright 2020-2021

@Modify Time      @Author    @Version    @Desciption
------------      -------    --------    -----------
2021/8/8 11:18 下午   haitao      0.0         None
'''
import numpy as np
import random

#添加高斯噪声
class GaussianNoiser(object):

    def __init__(self, mean=0.0, variance=1.0, amplitude=1.0, p=1):

        self.mean = mean
        self.variance = variance
        self.amplitude = amplitude
        self.p=p

    def __call__(self, img):
        """

        :param img: (h,w,c) of numpy anarrdy
        :return:
        """
        # if random.uniform(0, 1) < self.p:
        #     h, w, c = img.shape
        #     N = self.amplitude * np.random.normal(loc=self.mean, scale=self.variance, size=(h, w, 1))
        #     N = np.repeat(N, c, axis=2)
        #     img = N + img
        #     return img
        # else:
        #     return img
        if random.uniform(0, 1) < self.p:
            h, w, c = img.shape
            img_abs = np.sqrt(img[:,:,0]**2 + img[:,:,1]**2)
            mean_signal = self.get_s_mean(img_abs)
            snr = 35
            sigma_abs = mean_signal / (10 ** (snr / 20))
            sigma_real = sigma_abs / 0.66

            N = np.random.normal(loc=self.mean, scale=sigma_real, size=(h, w, 2))
            img = N + img
            return img

    def get_s_mean(self, img):
        pos_array = [
            118, 118
        ]
        w = 20
        h = 20

        start = pos_array[0]
        end = pos_array[1]
        roi_array = img[start:start + w, end:end + h]

        mean_signal = np.mean(roi_array)

        return mean_signal
