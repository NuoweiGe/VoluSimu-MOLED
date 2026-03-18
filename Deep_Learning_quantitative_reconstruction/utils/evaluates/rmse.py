# -*- encoding: utf-8 -*-
'''
@File    :   rmse.py  
@Contact :   913799761@qq.com
@License :   (C)Copyright 2022-2022

@Modify Time      @Author    @Version    @Desciption
------------      -------    --------    -----------
2022/1/10 8:14 PM   haitao      0.0         None
'''
import torch

def rmse(im_ori, im_rec):
    """

    :param im_ori:
    :param im_rec:
    :return:
    """

    rmse_error = (im_ori - im_rec) ** 2

    v = torch.sqrt(torch.mean(rmse_error, dim=(1, 2)))
    return v