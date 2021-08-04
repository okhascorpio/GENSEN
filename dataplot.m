function dataplot( arg )
%IMPORTDATA Summary of this function goes here
%   Detailed explanation goes here

% read data file
datafile = dlmread(arg);

% get dimention of data matrix
[dist_max,data_max]= size(datafile);

% seperate the rssi data and the packet loss information at end of every
% row
error_rate(:,1)= datafile(:,data_max);
for i=1:data_max-1
rssi_data(:,i) = datafile(:,i);
end

% get maximum and minimum point in rssi data
maxpt= max(rssi_data(:));
minpt= min(rssi_data(:));


% form zeros matrix for bar graph representation
bardata=zeros(dist_max,abs(minpt-maxpt));
% distance=1:dist_max;

for i=1:dist_max

    % find different values inside rssi data
    test_val=unique(rssi_data(i,:));
    % find number of repetition of above points
    test_rep=histc(rssi_data(i,:),test_val);
    % assign above data into bar matrix
    for j=1:length(test_val)
    bardata(i,abs(test_val(j))-abs(maxpt)+1)=test_rep(j);
    end


end


%============================================================%
%plot in 3D
figure(1);
h = bar3(bardata,0.5, 'detached');
set(gca,'xtick',1:length(h));
set(gca,'xticklabel',(minpt:maxpt));
for i = 1:length(h)
    zdata = ones(6*length(h),4);
    k = 1;
    for j = 1:dist_max-1
   
        zdata(j:j+6,:) = bardata(k,i);
        k = k+1;
   
    end
    set(h(i),'Cdata',zdata)
end

%colormap cool;
shading interp
for i = 1:length(h)
    zdata = get(h(i),'Zdata');
    set(h(i),'Cdata',zdata)
    set(h,'EdgeColor','k')
end

colormap (flipud(hot));
colorbar;

%============================================================%
% plot in 2D
figure(2);
pcolor(bardata);
set(gca,'xtick',1:length(h));
set(gca,'xticklabel',(minpt:maxpt));
 
colormap (flipud(hot));
colorbar;
