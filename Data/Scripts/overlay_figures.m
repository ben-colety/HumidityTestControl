% load saved figures
FileName = uigetfile('.fig');
fig1 = openfig(FileName);
FileName = uigetfile('.fig');
fig2   = openfig(FileName);
FileName = uigetfile('.fig');
fig3   = openfig(FileName);
FileName = uigetfile('.fig');
fig4   = openfig(FileName);
FileName = uigetfile('.fig');
fig5   = openfig(FileName);
%  Prepare 'subplot'
figure
h1=subplot(1,1,1);
h2=subplot(1,1,1);
h3=subplot(1,1,1);
h4=subplot(1,1,1);
h5=subplot(1,1,1);

copyobj(allchild(get(fig1,'CurrentAxes')),h1)
hold on
copyobj(allchild(get(fig2,'CurrentAxes')),h1)
copyobj(allchild(get(fig3,'CurrentAxes')),h1)
copyobj(allchild(get(fig4,'CurrentAxes')),h1)
copyobj(allchild(get(fig5,'CurrentAxes')),h1)