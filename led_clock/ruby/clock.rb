#!/usr/bin/env ruby

require 'optparse'
require 'rubygems'
require 'serialport'
require 'time'

options = {}
optparse = OptionParser.new do |opts|
  opts.on('-h', '--help', 'Display this message') do
    puts opts
    exit
  end
  opts.on('-p', '--port PORT', 'The serial port') do |p|
    puts p
    options[:port] = p;
  end
  opts.on('-s', '--set [TIME]', 'Sets the current date/time') do |t|
    if (t.nil?) then
      options[:time] = Time.now;
    else
      options[:time] = Time.parse t;
    end
    options[:action] = :set
  end
  opts.on('-c', '--check', 'Gets the date/time and displays the drift') do
    options[:action] = :check
  end
  opts.on('-t', '--tune [ADJUST]', Integer, 'Tune the timer by adding to the compare value') do |v|
    options[:action] = v.nil? ? :get_tune : :set_tune
    options[:adjustment] = v
  end
  
  opts.parse!
  
  if options[:port].nil? then
    puts 'port is required'
    puts opts
    exit
  end
  if options[:action].nil? then
    puts 'an action is required'
    puts opts
    exit
  end
  
  SerialPort.open(options[:port], 9600, 8, 1, SerialPort::NONE) do |sp|
    if options[:action] == :check
      sp.write 'G'
      n = Time.now
      a = sp.read(6).unpack('LS')
      seconds = a[0] + a[1] / 1000.0
      t = Time.at(seconds - n.gmt_offset)
      drift = ((t.to_f - n.to_f) * 1000).to_i
      puts "Computer #{n}"
      puts "AVR      #{t}"
      puts "Drift    #{drift} ms"
      if drift > 0 then puts "Running fast: increase tuning value"
      else puts "Running slow: decrease tuning value"
      end
    elsif options[:action] == :set
      sp.write 'S'
      t = Time.now
      sec = options[:time].to_i + options[:time].gmt_offset
      ms = (options[:time].to_f - options[:time].to_i) * 1000
      sp.write [sec, ms].pack('LS')
      puts sec
    elsif options[:action] == :set_tune
      sp.write 'T'
      sp.write [options[:adjustment]].pack('s')
    elsif options[:action] == :get_tune
      sp.write 'C'
      comp = sp.read(2).unpack('s')
      puts comp
    end
  end
end
