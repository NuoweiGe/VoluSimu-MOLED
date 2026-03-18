# -*- encoding: utf-8 -*-
'''
@File    :   rlne.py  
@Contact :   913799761@qq.com
@License :   (C)Copyright 2022-2022

@Modify Time      @Author    @Version    @Desciption
------------      -------    --------    -----------
2022/1/10 8:13 PM   haitao      0.0         None
'''
import torch

def rlne(im_ori, im_rec, dim):
    """
    evaluate the reconstruction error with relative L2 norm error
    :param im_ori: b*c*h*w of tensor
    :param im_rec: b*c*h*w of tensor
    :return: list of int
    """
    L2_error = im_ori - im_rec

    v = torch.norm(L2_error, p =2, dim=dim) / torch.norm(im_ori, p = 2, dim=dim)
    return v