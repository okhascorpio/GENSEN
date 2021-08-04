[dist_max,data_max]= size(testone);
for i=1:dist_max
test_sort(i,:)= sort(testone(i,:));
end
i=1;
j=1;
k=1;
while(i<=dist_max)
   while(j<=data_max)
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
                