function v = helics_time_property_rt_lead()
  persistent vInitialized;
  if isempty(vInitialized)
    vInitialized = helicsMEX(0, 1398230901);
  end
  v = vInitialized;
end