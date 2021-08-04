function importdata( arg )
%IMPORTDATA Summary of this function goes here
%   Detailed explanation goes here
datafile = dlmread(arg);

[dist_max,data_max]= size(datafile);

error_rate(:,1)= datafile(:,data_max);
for i=1:data_max-1
rssi_data(:,i) = datafile(:,i);
end
for i=1:dist_max
test_sort(i,:)= sort(rssi_data(i,:));
end
i=1;
j=1;
k=1;
while(i<=dist_max)
   while(j<data_max)
           %while(k<=data_max)
                scatter(i, test_sort(i,j));
                    hold on;                
                     j=j+1;

   end
    i=i+1;
    j=1;
    hold on;
end
grid on;
