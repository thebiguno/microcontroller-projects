#!/usr/bin/ruby

35.times do |i|
  puts "!!!!!!!!!!!!!! #{i} !!!!!!!!!!!!!!"
	actual = i
	tooth = 0
	
	while true do
	  tooth = tooth + 1
	  actual = actual + 1
	  
	  if [13,16,31].include? actual then
	    actual = actual + 2
	  end
	  
		if tooth == actual then
		  puts "match at #{tooth}"
			break
		end
	
		#puts "#{actual} != #{tooth}"
  		
  	if tooth > 35 then tooth = 0 end
  	if actual > 35 then actual = 0 end
		if [13,14,16,17,31,32].include? tooth then
		  puts "adjusting at #{tooth}"
			tooth = tooth + 1
		end
		
	end
end
