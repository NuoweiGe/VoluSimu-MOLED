# -*- encoding: utf-8 -*-
'''
@File    :   dtype.py  
@Contact :   913799761@qq.com
@License :   (C)Copyright 2022-2022

@Modify Time      @Author    @Version    @Desciption
------------      -------    --------    -----------
2022/1/12 4:31 PM   haitao      0.0         None
'''
import torch
_integer_types = (torch.int8, torch.uint8,      # 8 bits
                  torch.short, torch.int,       # 8 bits
                  torch.int16, torch.int32,     # 16 or 32 or 64 bits
                  torch.int64, torch.long,      # 64 bits
                  )
_integer_ranges = {t: (torch.iinfo(t).min, torch.iinfo(t).max)
                   for t in _integer_types}
dtype_range = {torch.bool: (False, True),
               torch.float: (-1, 1),
               torch.float16: (-1, 1),
               torch.float32: (-1, 1),
               torch.float64: (-1, 1)}
dtype_range.update(_integer_ranges)

_supported_types = list(dtype_range.keys())