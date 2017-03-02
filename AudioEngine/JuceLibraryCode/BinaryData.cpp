/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

namespace BinaryData
{

//================== processor.lua ==================
static const unsigned char temp_binary_data_0[] =
"\n"
"local ffi = require(\"ffi\")\n"
"\n"
"-- Declares the Lua buffers for audio processing\n"
"local i0 = ffi.new(\"float[?]\",1)\n"
"local i1 = ffi.new(\"float[?]\",1)\n"
"\n"
"local o0 = ffi.new(\"float[?]\",1)\n"
"local o1 = ffi.new(\"float[?]\",1)\n"
"\n"
"-- default audio processor\n"
"local context = { phase = 0 }\n"
"local unit =\n"
"   {\n"
"      -- this processor renders an aliased saw wave at 440 Hz\n"
"      process =\n"
"         function(context)\n"
"            local rate = 440 / 44100\n"
"            context.phase = context.phase + rate\n"
"            if context.phase > 1.0 then\n"
"               context.phase = context.phase - 1.0\n"
"            end\n"
"            return math.sin(context.phase * 6.28318530718)\n"
"         end,\n"
"      noteOn        = function(context, note, velocity, channel) end,\n"
"      noteOff       = function(context, note, channel)           end,\n"
"      controlChange = function(context, control, value, channel) end,\n"
"      init          = function() return { phase = 0 }            end,\n"
"      default       = function(context) return context           end,\n"
"      config        = { inputs = 0, outputs = 1, noteon_inputs = 4, noteoff_inputs = 3, controlchange_inputs = 4, is_active = true }\n"
"   }\n"
"\n"
"-- Globals an flags\n"
"local new_unit = {}\n"
"local new_context = {}\n"
"local ready_to_swap = false\n"
"\n"
"-- receives the input audio buffer pointers\n"
"function setInputBuffers(l,r)\n"
"   i0 = ffi.cast(\"float *\",l)\n"
"   i1 = ffi.cast(\"float *\",r)\n"
"end\n"
"\n"
"-- receives the output audio buffer pointers\n"
"function setOutputBuffers(l,r)\n"
"   o0 = ffi.cast(\"float *\",l)\n"
"   o1 = ffi.cast(\"float *\",r)\n"
"end\n"
"\n"
"-- calls the current unit noteOn function\n"
"function noteOn(note, velocity, channel)\n"
"   unit.noteOn(context, note, velocity, channel)\n"
"end\n"
"\n"
"-- calls the current unit noteOff function\n"
"function noteOff(note, channel)\n"
"   unit.noteOff(context, note, channel)\n"
"end\n"
"\n"
"-- keeps the controls changes to restore them later\n"
"local control_table = {{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}}\n"
"\n"
"-- calls the current unit controlChange function\n"
"function controlChange(control, value, channel)\n"
"   control_table[channel][control] = value\n"
"   unit.controlChange(context, control, value, channel)\n"
"end\n"
"\n"
"-- restore all control changes to the current unit\n"
"function restoreControls()\n"
"   for channel, controls in ipairs(control_table) do\n"
"  \t\tfor control, value in ipairs(controls) do\n"
"        unit.controlChange(context, control, value, channel)\n"
"      end\n"
"\tend\n"
"end\n"
"\n"
"-- changes the unit\n"
"function swap()\n"
"   unit = new_unit\n"
"   context = new_context\n"
"   restoreControls()\n"
"   ready_to_swap = false\n"
"end\n"
"\n"
"-- returns the corresponding outputs based on the number of output configuration\n"
"function getOutputs(y0,y1, noutputs)\n"
"   if noutputs == 0 then\n"
"      return 0, 0\n"
"   elseif noutputs == 1 then\n"
"      return y0, y0\n"
"   else\n"
"      return y0, y1\n"
"   end\n"
"end\n"
"\n"
"-- main function to process the audio\n"
"function process(size)\n"
"   -- changes the unit if new code has been loaded\n"
"   if ready_to_swap then swap() end\n"
"   -- small hack to workaround different inputs and outputs\n"
"   local x0,x1,x2,x3,x4,x5,x6 = 0,0,0,0,0,0\n"
"   local y0,y1,y2,y3,y4,y5,y6 = 0,0,0,0,0,0\n"
"   local noutputs = unit.config.outputs\n"
"   -- if active, pass the context\n"
"   if unit.config.is_active then\n"
"      for i=0, size-1 do\n"
"         x0 = i0[i]\n"
"         x1 = i1[i]\n"
"         y0,y1,y2,y3,y4,y5,y6 = unit.process(context,x0,x1,x2,x3,x4,x5,x6)\n"
"         o0[i],o1[i] = getOutputs(y0,y1,noutputs)\n"
"      end\n"
"   else\n"
"      for i=0, size-1 do\n"
"         x0 = i0[i]\n"
"         x1 = i1[i]\n"
"         y0,y1,y2,y3,y4,y5,y6 = unit.process(x0,x1,x2,x3,x4,x5,x6)\n"
"         o0[i],o1[i] = getOutputs(y0,y1,noutputs)\n"
"      end\n"
"   end\n"
"end\n"
"\n"
"-- displays the list of errors\n"
"function show_errors(errors)\n"
"\tfor i, error in ipairs(errors) do\n"
"  \t\tprint (error.msg)\n"
"\tend\n"
"end\n"
"\n"
"-- loads a string of lua code\n"
"function loadCode(code)\n"
"    local result = loadstring(code)()\n"
"    if #result.error > 0 then\n"
"        show_errors(result.error)\n"
"    else\n"
"        new_unit = loadstring(result.code)()\n"
"        if not (new_unit and new_unit.init) then\n"
"            return 0\n"
"        else\n"
"            new_context = new_unit.init()\n"
"            new_unit.default(new_context)\n"
"            ready_to_swap = true\n"
"            return 1\n"
"        end\n"
"    end\n"
"end\n";

const char* processor_lua = (const char*) temp_binary_data_0;


const char* getNamedResource (const char*, int&) throw();
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes) throw()
{
    unsigned int hash = 0;
    if (resourceNameUTF8 != 0)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0x7e6f86cb:  numBytes = 4138; return processor_lua;
        default: break;
    }

    numBytes = 0;
    return 0;
}

const char* namedResourceList[] =
{
    "processor_lua"
};

}
