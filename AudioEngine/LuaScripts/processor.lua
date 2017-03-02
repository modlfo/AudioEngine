--[[
The MIT License (MIT)

Copyright (c) 2017 Leonardo Laguna Ruiz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
]]--

local ffi = require("ffi")

-- Declares the Lua buffers for audio processing
local i0 = ffi.new("float[?]",1)
local i1 = ffi.new("float[?]",1)

local o0 = ffi.new("float[?]",1)
local o1 = ffi.new("float[?]",1)

-- default audio processor
local context = { phase = 0 }
local unit =
   {
      -- this processor renders an aliased saw wave at 440 Hz
      process =
         function(context)
            local rate = 440 / 44100
            context.phase = context.phase + rate
            if context.phase > 1.0 then
               context.phase = context.phase - 1.0
            end
            return math.sin(context.phase * 6.28318530718)
         end,
      noteOn        = function(context, note, velocity, channel) end,
      noteOff       = function(context, note, channel)           end,
      controlChange = function(context, control, value, channel) end,
      init          = function() return { phase = 0 }            end,
      default       = function(context) return context           end,
      config        = { inputs = 0, outputs = 1, noteon_inputs = 4, noteoff_inputs = 3, controlchange_inputs = 4, is_active = true }
   }

-- Globals an flags
local new_unit = {}
local new_context = {}
local ready_to_swap = false

-- receives the input audio buffer pointers
function setInputBuffers(l,r)
   i0 = ffi.cast("float *",l)
   i1 = ffi.cast("float *",r)
end

-- receives the output audio buffer pointers
function setOutputBuffers(l,r)
   o0 = ffi.cast("float *",l)
   o1 = ffi.cast("float *",r)
end

-- calls the current unit noteOn function
function noteOn(note, velocity, channel)
   unit.noteOn(context, note, velocity, channel)
end

-- calls the current unit noteOff function
function noteOff(note, channel)
   unit.noteOff(context, note, channel)
end

-- keeps the controls changes to restore them later
local control_table = {{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}}

-- calls the current unit controlChange function
function controlChange(control, value, channel)
   control_table[channel][control] = value
   unit.controlChange(context, control, value, channel)
end

-- restore all control changes to the current unit
function restoreControls()
   for channel, controls in ipairs(control_table) do
  		for control, value in ipairs(controls) do
        unit.controlChange(context, control, value, channel)
      end
	end
end

-- changes the unit
function swap()
   unit = new_unit
   context = new_context
   restoreControls()
   ready_to_swap = false
end

-- returns the corresponding outputs based on the number of output configuration
function getOutputs(y0,y1, noutputs)
   if noutputs == 0 then
      return 0, 0
   elseif noutputs == 1 then
      return y0, y0
   else
      return y0, y1
   end
end

-- main function to process the audio
function process(size)
   -- changes the unit if new code has been loaded
   if ready_to_swap then swap() end
   -- small hack to workaround different inputs and outputs
   local x0,x1,x2,x3,x4,x5,x6 = 0,0,0,0,0,0
   local y0,y1,y2,y3,y4,y5,y6 = 0,0,0,0,0,0
   local noutputs = unit.config.outputs
   -- if active, pass the context
   if unit.config.is_active then
      for i=0, size-1 do
         x0 = i0[i]
         x1 = i1[i]
         y0,y1,y2,y3,y4,y5,y6 = unit.process(context,x0,x1,x2,x3,x4,x5,x6)
         o0[i],o1[i] = getOutputs(y0,y1,noutputs)
      end
   else
      for i=0, size-1 do
         x0 = i0[i]
         x1 = i1[i]
         y0,y1,y2,y3,y4,y5,y6 = unit.process(x0,x1,x2,x3,x4,x5,x6)
         o0[i],o1[i] = getOutputs(y0,y1,noutputs)
      end
   end
end

-- displays the list of errors
function show_errors(errors)
	for i, error in ipairs(errors) do
  		print (error.msg)
	end
end

-- loads a string of lua code
function loadCode(code)
    local result = loadstring(code)()
    if #result.error > 0 then
        show_errors(result.error)
    else
        new_unit = loadstring(result.code)()
        if not (new_unit and new_unit.init) then
            return 0
        else
            new_context = new_unit.init()
            new_unit.default(new_context)
            ready_to_swap = true
            return 1
        end
    end
end
