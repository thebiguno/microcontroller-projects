#!/usr/bin/env ruby

require 'optparse'
require 'rubygems'
require 'serialport'

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
  opts.on('-s', '--set', 'Sets the current date/time') do
    options[:action] = :set
  end
  opts.on('-c', '--check', 'Gets the date/time and displays the drift') do
    options[:action] = :check
  end
  opts.on('-t', '--tune ADJUST', 'Tune the timer by adding to the compare value') do |v|
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
      millis = sp.read(4).unpack('L'); # convert 4 byte string into a uint32_t
      puts (Time.now.to_i).floor - millis.to_i
    elsif options[:action] == :set
      sp.write 'S'
      sp.write [((Time.now.to_i).floor].pack('L')
    elsif options[:action] == :tune
      sp.puts 'T'
      sp.puts 
    end
  end
end
