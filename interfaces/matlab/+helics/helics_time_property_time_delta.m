function v = helics_time_property_time_delta()
  persistent vInitialized;
  if isempty(vInitialized)
    vInitialized = helicsMEX(0, 1398230897);
  end
  v = vInitialized;
end