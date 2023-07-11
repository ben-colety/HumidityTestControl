clear all;
FileName = uigetfile('.fig');
openfig(FileName);
% brush and create variables step_full, offset, final and data tip
% cursor_info which corresponds to the start of the step
%%
pause;
offset_avg = mean(offset(:,2))
final_avg = mean(final(:,2))
%step_an = stepinfo(step_full(:,2), step_full(:,1), final_avg, offset_avg)
%Time Constant
%if(final_avg > offset_avg)
    tau63_idx = find(abs(step_full(:,2)-final_avg)>=0.37*abs(final_avg-offset_avg),1,"last");
%else
    %tau63_idx = find(abs(step_full(:,2)-final_avg)>=0.37*abs(final_avg-offset_avg),1,"last");
%end
tau63_ms = step_full(tau63_idx,1) - cursor_info.Position(1,1)
%%
%Rate of Change - between minimum + 10% and tau63
start_10_idx = find(abs(step_full(:,2)-offset_avg)>=1.1*offset_avg,1,"first");
step_fit =  polyfit([step_full(start_10_idx,1), step_full(tau63_idx,1)],[step_full(start_10_idx,2), step_full(tau63_idx,2)],1);
rate_of_change = step_fit(1,1) % %RH/ms