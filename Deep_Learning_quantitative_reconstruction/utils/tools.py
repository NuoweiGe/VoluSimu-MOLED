# -*- encoding: utf-8 -*-
'''
@File    :   tools.py
@Contact :   913799761@qq.com
@License :   (C)Copyright 2020-2021

@Modify Time      @Author    @Version    @Desciption
------------      -------    --------    -----------
2021/7/23 3:11 下午   haitao      0.0         None
'''
import os
import numpy as np
import scipy.io as scio
from collections import OrderedDict

def mkdir(path):
    """create a single empty directory if it didn't exist
    Parameters:
        path (str) -- a single directory path
    """
    if not os.path.exists(path):
        os.makedirs(path)

def charles2mat(charles_path, h, w, c):

    data_in = np.fromfile(charles_path, dtype=np.float32)
    data_pairs = data_in.reshape(h, w, c)

    mat = OrderedDict()
    mat["charles"] = data_pairs

    file_path = charles_path+".mat"

    scio.savemat(file_path, data_pairs)
    return
