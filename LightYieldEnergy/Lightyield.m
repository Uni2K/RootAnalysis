x1=LightYields.x;
y1=LightYields.y1; 
y2=LightYields.y2;
y3=LightYields.y3;


s1=scatter(x1,y1,70,'filled','s','red');
[f, gof] =fit(x1,y1,'exp1');
coefficientValues = coeffvalues(f)
hold on;
h1=plot(f,x1,y1);
h1(1).Marker='none'
h1(2).Color='red'

hold on
s2=scatter(x1,y2,70,'filled','d','b')
[f1, gof1] =fit(x1,y2,'exp1');
coefficientValues1 = coeffvalues(f1)
hold on
h2=plot(f1,x1,y2)
h2(1).Marker='none'
h2(2).Color='b'
hold on

s3=scatter(x1,y3,70,'filled','g')
[f2, gof2] =fit(x1,y3,'exp1');
coefficientValues2 = coeffvalues(f2)

hold on
h3=plot(f2,x1,y3)
h3(1).Marker='none'
h3(2).Color='g'


str1 = sprintf('1.4GeV      a=%.3f      b=%.3f   \\chi^2/ndf= %.3f',coefficientValues(1,1),coefficientValues(1,2),gof.adjrsquare);
str2 = sprintf('2.6GeV      a=%.3f      b=%.3f   \\chi^2/ndf= %.3f',coefficientValues1(1,1),coefficientValues1(1,2),gof1.adjrsquare);
str3 = sprintf('5.2GeV      a=%.3f      b=%.3f   \\chi^2/ndf= %.3f',coefficientValues2(1,1),coefficientValues2(1,2),gof2.adjrsquare);


leg=legend([s1 s2 s3 ],{str1,str2,str3},'Location','best', 'Fontsize',20)
leg.NumColumns = 1;
leg.Title.String=' f(x)=a*exp(b*x) '
leg.EdgeColor='#9ea3a0'


hold on
x1=LightYields1.x1;
y1=LightYields1.y1; 
y2=LightYields1.y2;
y3=LightYields1.y3;


s1=scatter(x1,y1,70,'filled','s','red');
f=fit(x1,y1,'exp1');
hold on;
h1=plot(f,x1,y1);
h1(1).Marker='none'
h1(2).Color='red'

hold on
s2=scatter(x1,y2,70,'filled','d','b')
f1=fit(x1,y2,'exp1')
hold on
h2=plot(f1,x1,y2)
h2(1).Marker='none'
h2(2).Color='b'
hold on

s3=scatter(x1,y3,70,'filled','g')
f2=fit(x1,y3,'exp1')
hold on
h3=plot(f2,x1,y3)
h3(1).Marker='none'
h3(2).Color='g'

ylabel('light yield [N_{pe}]') 
xlabel('distance [mm]') 
set(gca,'FontSize',20)





