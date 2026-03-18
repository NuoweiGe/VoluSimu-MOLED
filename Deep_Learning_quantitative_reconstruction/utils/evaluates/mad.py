# -*- encoding: utf-8 -*-
'''
@File    :   mad.py  
@Contact :   913799761@qq.com
@License :   (C)Copyright 2022-2022

@Modify Time      @Author    @Version    @Desciption
------------      -------    --------    -----------
2022/1/10 8:13 PM   haitao      0.0         None
'''
import torch

def mad(im_ori, im_rec):
    """
    :param im_ori: b*c*h*w of tensor
    :param im_rec: b*c*h*w of tensor
    :return: list of int
    """
    mad_error = im_ori - im_rec

    v = torch.mean(torch.abs(mad_error), dim=(1,2))
    return v