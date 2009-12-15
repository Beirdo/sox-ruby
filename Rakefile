require "rake"
require "rake/rdoctask"
require "rake/testtask"
require "rake/gempackagetask"

Rake::RDocTask.new do |rd|
  rd.main = "README"
  rd.rdoc_files.include("README", "{lib/**/*.rb,ext/**/*.c}")
  rd.options = ["--line-numbers", "--inline-source", "--title", "lame-ruby",
                "--main", "README"]
  rd.rdoc_dir = "docs/rdoc"
end

Rake::TestTask.new do |t|
  t.libs << "ext"
  t.libs << "lib"
  t.libs << "test"
  t.test_files = FileList["test/**/*_{helper,test}.rb"]
end

lame_ruby_version = File.read(File.expand_path("VERSION", File.dirname(__FILE__))).to_a.first.chomp

spec = Gem::Specification.new do |s|
  s.name = "sox-ruby"
  s.version = sox_ruby_version
  s.summary = "libsox wrapper for Ruby."
  s.description = <<EOS
sox-ruby is a LAME wrapper for Ruby.
This will allow for sound file conversions and effects from Ruby.
This is a Ruby extension library.
EOS
  s.author = "Gavin Hurlbut"
  s.email = "gjhurlbu@gmail.com"
  s.homepage = "http://github.com/Beirdo/sox-ruby"

  s.files = Dir.glob("{README,LICENCE,VERSION,ChangeLog,setup.rb,{test,lib}/**/*.rb,ext/**/{*.c,extconf.rb}}")
  
  s.extensions = ["ext/extconf.rb"]

  s.test_files = Dir.glob("test/**/*_test.rb")

  s.require_path = "ext"
  s.autorequire = "sox"

  s.has_rdoc = true
  s.rdoc_options =
    ["--line-numbers", "--inline-source", "--title", "lame-ruby",
     "--main", "README"]
  s.extra_rdoc_files = ["README"]
end

Rake::GemPackageTask.new(spec) do |pkg|
  pkg.need_zip = true
  pkg.need_tar = true
end

ruby_path = File.join(RbConfig::CONFIG["bindir"],
                      RbConfig::CONFIG["ruby_install_name"])

namespace :setup do
  desc "run 'config' task in setup.rb."
  task :config do
    system("#{ruby_path} setup.rb config #{ENV["SETUP_OPTS"]}")
  end
  
  desc "run 'setup' task in setup.rb."
  task :setup do
    system("#{ruby_path} setup.rb setup #{ENV["SETUP_OPTS"]}")
  end
  
  desc "run 'install' task in setup.rb."
  task :install do
    system("#{ruby_path} setup.rb install #{ENV["SETUP_OPTS"]}")
  end

  desc "run 'test' task in setup.rb."
  task :test do
    system("#{ruby_path} setup.rb test #{ENV["SETUP_OPTS"]}")
  end

  desc "run 'clean' task in setup.rb."
  task :clean do
    system("#{ruby_path} setup.rb clean #{ENV["SETUP_OPTS"]}")
  end

  desc "run 'distclean' task in setup.rb."
  task :distclean do
    system("#{ruby_path} setup.rb distclean #{ENV["SETUP_OPTS"]}")
  end

  desc "run default tasks in setup.rb."
  task :all do
    system("#{ruby_path} setup.rb #{ENV["SETUP_OPTS"]}")
  end
end

task :default => :test
