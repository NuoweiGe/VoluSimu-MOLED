# SMRI-VoluSimu (Simulation Artist of MRI) platform
**Version:** 3.1.1 
**Release Date:** **2026-03-16**  
**Contributors:**  [Haitao Huang](https://github.com/hei6775), [Qinqin Yang](https://github.com/qinqinyang) and [Nuowei Ge](https://github.com/NuoweiGe)  
**Affiliation:**  [**XMU-SMRI Lab**](https://smri.xmu.edu.cn), Department of Electronic Science, Xiamen University, China  

# 1. What is SMRI
**SMRI** (*Simulation Artist of MRI*) is a **powerful framework** for rapid **MRI simulation**.  It is designed to be **easy to learn and use**, while delivering **high performance** and **high quality** with minimal code.  Most of the SMRI code base is written in **C++** for efficiency, and it leverages **GPU acceleration** via **CUDA** to perform complex magnetic resonance imaging simulations at scale.  

**Note:** This compiled version is intended **exclusively** for **VoluSimu-MOLED** and is **not a general-purpose release**.

---

**Reference:**  
Yang QQ, Huang HT, Yong HT, *et al.* **SMRI: Next-generation MRI simulation platform for training data generation in the era of AI.** In *Proceedings of the 33rd Joint ISMRM & ISMRT Annual Meeting*. Honolulu, Hawaii, US; 2025. p. 0009.

# 2. Environment Setup

Modify the environment variables
`/home/your_user_name/.bashrc` according to the actual setup:
```bash
# cuda
export PATH="/usr/local/cuda-12.2/bin:$PATH"
export LD_LIBRARY_PATH="/usr/local/cuda-12.2/lib64:$LD_LIBRARY_PATH"
# MATLAB
export PATH="/home/yqq/Env/MATLAB/bin/:$PATH"
# cmake
export PATH="/home/yqq/Env/cmake-3.24.0-linux-x86_64/bin:$PATH"
# gcc
export LD_LIBRARY_PATH="/home/yqq/Downloads/gcc/lib/:$LD_LIBRARY_PATH"
export PATH="/home/yqq/Downloads/gcc/bin:/home/yqq/Downloads/gcc/lib64:$PATH"
```
Use source to apply the changes
```bash
source /home/your_user_name/.bashrc
```

# 3. How to Run
You can directly execute the following command in the build directory:
```bash
./simu_bin
```
You can also run it in the background using:
```bash
nohup ./simu_bin >../my.log 2>&1 &
```

# 4. Key Parameter Description (mainCfg.json) 
```json
{
    // Sequence Name
    "SeqName": "GREOLEDM2",

    // Directory of Virtual Imaging Objects (VObj)
    "SrcRoot": "/data/qinqin/code_SMRI/VVtry/Simu_pedatric_T2star_3D_64choice32slice/",

    //Directory of Simulated Output Signals
    "DstRoot": "/data/qinqin/code_SMRI/VVtry/simu_out/output/",

    // Number of Generated Samples
    "FileNum": 1,

    // Number of Simulated Spins
    "SpinNum": 1,
    "VoxelPerThread": 20,
    "VoxelPerKernel": 13107200,

    // Starting File Index
    "StartFileIndex": 0,

    // GPU Index
    "GPUID": 0,

    // Dimensions of the generated k-space
    "XN": 128,
    "YN": 128,
    "ZN": 1,

    // Number of grids of the virtual imaging object
    "ModelXN": 512,
    "ModelYN": 512,
    "ModelZN": 32,

    // Resolution of the virtual imaging object (m)
    "ModelXRes": 0.00044,
    "ModelYRes": 0.00044,
    "ModelZRes": 0.000125,

    // Number of k-space
    "KSpaceNum": 2
}
```

