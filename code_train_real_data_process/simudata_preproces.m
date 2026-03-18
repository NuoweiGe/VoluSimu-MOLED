% Make Charles for network training
% Date: 2022-10-24
% User: Qinqin Yang
% --------------------------

clear;clc;

srcUrl = '';
vobjUrl = '';

dstUrl = '';

topname='Brain_';

mkdir(dstUrl);
FRE_NUM = 128;
PHASE_NUM = FRE_NUM;
h=FRE_NUM;
w=h-2;
scale_factor=1;
model_h=512;
EXPAND_NUM = 256;
gy=2.67519e8;
pi=3.14159265359;

start_index = 1;
end_index = 1;

for index = start_index:end_index
    
    srcName = sprintf("%d.out", index);
    vobjName = sprintf("%d.mat", index);
    
    srcPath = fullfile(srcUrl, srcName);
    vobjPath = fullfile(vobjUrl, vobjName);
    
    load(vobjPath);
    
    current_out = zeros(3, EXPAND_NUM, EXPAND_NUM);
    
    T2star = VObj.T2Star;
    T2star = abs(imresize(T2star,[EXPAND_NUM,EXPAND_NUM],'nearest'));
    T2star =( rot90(T2star,-1));
    

    alldata = SMri2D_reader(srcPath, FRE_NUM, PHASE_NUM);
    
    % echo1
    ksp_echo1 = zeros(FRE_NUM,FRE_NUM);
    k1_real_data = reshape(alldata(1,:,:,1), [FRE_NUM,PHASE_NUM]);
    k1_imag_data = reshape(alldata(2,:,:,1), [FRE_NUM,PHASE_NUM]);
    k1_ori_kspace = k1_real_data + 1i*k1_imag_data;
    k1_ori_kspace(:,2:2:end) = flipud(k1_ori_kspace(:,2:2:end));
    ksp_echo1(:,2:PHASE_NUM-1) = k1_ori_kspace(:,1:w);
        
    % echo2
    ksp_echo2 = zeros(FRE_NUM,FRE_NUM);
    k2_real_data = reshape(alldata(1,:,:,2), [FRE_NUM,PHASE_NUM]);
    k2_imag_data = reshape(alldata(2,:,:,2), [FRE_NUM,PHASE_NUM]);
    k2_ori_kspace = k2_real_data + 1i*k2_imag_data;
    k2_ori_kspace(:,2:2:end) = flipud(k2_ori_kspace(:,2:2:end));
    ksp_echo2(:,2:PHASE_NUM-1) = k2_ori_kspace(:,1:w);

    
    % add noise in kspace
    ksp_echo_img1 = ifft2c(ksp_echo1);
    cup = max(abs(ksp_echo_img1(:)));
    ksp_echo_img1 = ksp_echo_img1 ./ cup;

    ksp_echo_img2 = ifft2c(ksp_echo2);
    ksp_echo_img2 = ksp_echo_img2 ./ cup;
    
  
    
    rand_factor=(0.015*rand()+0.002);
    k1_noise = rand_factor*(rand(FRE_NUM,FRE_NUM)-0.5)+rand_factor*1.0i*(rand(FRE_NUM,FRE_NUM)-0.5);
    ksp_img_noise1 = ksp_echo_img1 + k1_noise;
    ksp_echo1 = fft2c(ksp_img_noise1);

    ksp_img_noise2 = ksp_echo_img2 + k1_noise;
    ksp_echo2 = fliplr(fft2c(ksp_img_noise2)); % for GRE-MOLED
    %ksp_echo2 = fliplr(fft2c(ksp_img_noise2)); % for SE-MOLED
       
    k_expand = zeros(EXPAND_NUM, EXPAND_NUM,2) + 1.0i * zeros(EXPAND_NUM, EXPAND_NUM,2);
    k_expand(round((EXPAND_NUM-FRE_NUM)/2)+1:round((EXPAND_NUM+FRE_NUM)/2),round((EXPAND_NUM-FRE_NUM)/2)+1:round((EXPAND_NUM+FRE_NUM)/2),1)=ksp_echo1;
    k_expand(round((EXPAND_NUM-FRE_NUM)/2)+1:round((EXPAND_NUM+FRE_NUM)/2),round((EXPAND_NUM-FRE_NUM)/2)+1:round((EXPAND_NUM+FRE_NUM)/2),2)=ksp_echo2;
    k_image=ifft2c(k_expand);    

    k_max3norm = max(abs(k_image(:)));
    k_image = scale_factor*k_image/k_max3norm;
    
    current_out(1,:,:) = real(k_image(:,:,1));
    current_out(2,:,:) = imag(k_image(:,:,1));
    current_out(3,:,:) = real(k_image(:,:,2));
    current_out(4,:,:) = imag(k_image(:,:,2));
    current_out(5,:,:) = (10*T2star(:,:,1));

    
    if sum(isnan(current_out(:)))==0
        % save file to Charles
        save_name = [topname,num2str(index,'%02d'),'.Charles'];
        save_path = fullfile(dstUrl, save_name);
        
        [fid,msg]=fopen(save_path, 'wb');
        fwrite(fid, current_out, 'single');
        fclose(fid);
        disp(save_path);
    else
        disp('error');
    end
end