R"(
function data.extend (target, new_data)
  -- append rows from new_data to target
  -- TODO: handle named table rows
  for i=1,#new_data do
        target[#target+1] = new_data[i]
    end
end
)"
