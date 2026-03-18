clear;clc;

src_url='';
dst_url='';
if ~exist(dst_url,'dir')==1
    mkdir(dst_url);
end
startLayer = 1;
endLayer = 21;

max1=zeros(1,endLayer-startLayer+1);
phase_num = 128;
fre_num = phase_num-2;
expand_num = 256;
max_m=zeros(1,endLayer-startLayer+1);
for layer = startLayer:endLayer
    if layer<10
    filename = sprintf('Brain_layer_0%d.mat',layer)
    end
    if layer<100 & layer>=10
        filename = sprintf('Brain_layer_%d.mat',layer)
    end
    if layer>=100
        filename = sprintf('Brain_layer20_fra_%d.mat',layer)
    end

    filepath = fullfile(src_url, filename);
    
    load(filepath);
    
    output = zeros(6, expand_num, expand_num);

    k1 = zeros(phase_num, fre_num, 'like', 1+2j);
    k2 = zeros(phase_num, fre_num, 'like', 1+2j);

    k1(:,1:fre_num) = complex_k2(:,:,1);

    k2(:,1:fre_num) = complex_k2(:,:,2);

    expand_k1 = zeros(expand_num, expand_num, 'like', 1+2j);
    expand_k2 = zeros(expand_num, expand_num, 'like', 1+2j);

    expand_k1(round((expand_num-phase_num)/2)+1:round((expand_num+phase_num)/2),round((expand_num-fre_num)/2)+1:round((expand_num+fre_num)/2),:)=k1;
    expand_k2(round((expand_num-phase_num)/2)+1:round((expand_num+phase_num)/2),round((expand_num-fre_num)/2)+1:round((expand_num+fre_num)/2),:)=k2;

    % img1 = (ifft2c((expand_k1)));% for SE-MOLED
    % img2 = (ifft2c((expand_k2)));% for SE-MOLED
    img1 = (ifft2c(flipud(expand_k1)));% for GRE-MOLED
    img2 = ((ifft2c(flipud(fliplr(expand_k2)))));% for GRE-MOLED
    img1_max =max(abs(img1(:)))

    img1 = img1/img1_max;
    img2 = img2/img1_max;


    output(1,:,:) = real(img1);
    output(2,:,:) = imag(img1);
    output(3,:,:) = real(img2);
    output(4,:,:) = imag(img2);
    output(5,:,:) = real(img3);
    output(6,:,:) = imag(img3);

    savename = sprintf("layer_%02d.Charles", layer);
    savepath = fullfile(dst_url, savename);

    [fid, msg] = fopen(savepath, 'wb');
    fwrite(fid, output, 'single');
    fclose(fid);
end


