#!/usr/local/bin/ruby
#coding:UTF-8
begin

algorithms = File.read("algorithms.list").split(?\n)

size = 0

algorithms.each_with_index do |a, i|
	if a.size >= size
		size = a.size
	end
	puts "void flame_#{a.downcase}(double* x, double* y) // #{a} (Variation #{i})
{
    FLAME_THETA; FLAME_R; FLAME_OLDX;
    
}

"
end
algorithms.each_with_index do |a, i|
	puts "void #{"flame_#{a.downcase}".ljust(size+6)}(double* x, double* y); // #{a.ljust(size)} (Variation #{i})
"
end


algorithms.each_with_index do |a, i|
	puts "\t0.5, // #{a}
"
end


algorithms.each_with_index do |a, i|
	puts "\tflame_#{a.downcase}, // #{i}
"
end


algorithms.each_with_index do |a, i|
	puts "\t\"#{a}\",
"
end


	puts "+'<a href=\"javascript:repondre(\\'add_algorithm random\\', 404)\">Random</a> (Variation fixée aléatoirement)<br>'\n"
algorithms.each_with_index do |a, i|
	puts "+'<a href=\"javascript:repondre(\\'add_algorithm #{a.downcase}\\', 404)\">#{a}</a> (Variation #{i})<br>'\n"
end

rescue Exception; puts $!
ensure puts "Done."; gets end
