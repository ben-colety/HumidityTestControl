% manually import data files
sensor.dry_dig = mean(fdcdry(:,2));
sensor.dry_RH = mean(bmedry(:,2));
sensor.wet_dig = mean(fdcwet(:,2));
sensor.wet_RH = mean(bmewet);
%%
linear = polyfit([sensor.dry_RH, sensor.wet_RH],[sensor.dry_dig, sensor.wet_dig],1);
sensor.dry_dig = linear(2);
sensor.dry_RH = 0;