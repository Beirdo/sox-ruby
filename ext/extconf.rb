require 'mkmf'

extension_name = 'sox'

dir_config(extension_name)

if have_library("sox", "sox_format_init") && have_header("sox.h")
  create_makefile(extension_name)
end
