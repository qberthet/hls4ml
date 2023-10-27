import glob
import os
from shutil import copy

from hls4ml.writer.vivado_writer import VivadoWriter


class VitisAcceleratorWriter(VivadoWriter):
    def __init__(self):
        super().__init__()

    def write_nnet_utils_overrides(self, model):
        ###################
        # nnet_utils
        ###################

        filedir = os.path.dirname(os.path.abspath(__file__))

        srcpath = os.path.join(filedir, '../templates/vitis_accelerator/nnet_utils/')
        dstpath = f'{model.config.get_output_dir()}/firmware/nnet_utils/'

        headers = [os.path.basename(h) for h in glob.glob(srcpath + '*.h')]

        for h in headers:
            copy(srcpath + h, dstpath + h)
    
    def write_build_script_backend_override(self, model):
#        try:
#
##            filedir = os.path.dirname(os.path.abspath(__file__))
##            writerspath = os.path.join(filedir, '../writer')
##            vitis_accelerator_writer_file = writerspath + "vitis_accelerator_writer.py"
#            with open(model.config.get_output_dir() + '/project.tcl', 'r') as file:
#                lines = file.readlines()
#                print("[K]\n")
#                print(lines)
#
#            with open(model.config.get_output_dir() + '/project.tcl', 'r', 'w') as file:
#                print ('[K] overriding project.tcl')
#                for line in lines:
#                    if 'backend' in line:
#                        index = line.find('backend')
#                        line = line[:index] + 'backend ' + 'vitisaccelerator' + '\n'
#                    file.write(line)
#        
#        except FileNotFoundError:
#            print("File project.tcl not found.")

        # project.tcl
        f = open(f'{model.config.get_output_dir()}/project.tcl', 'w')
        f.write('variable project_name\n')
        f.write(f'set project_name "{model.config.get_project_name()}"\n')
        f.write('variable backend\n')
        f.write('set backend "vitisaccelerator"\n')
        f.write('variable part\n')
        f.write('set part "{}"\n'.format(model.config.get_config_value('Part')))
        f.write('variable clock_period\n')
        f.write('set clock_period {}\n'.format(model.config.get_config_value('ClockPeriod')))
        f.write('variable clock_uncertainty\n')
        f.write('set clock_uncertainty {}\n'.format(model.config.get_config_value('ClockUncertainty', '12.5%')))
        f.close()

    def write_kernel(self, model):
        """Write the Python-C++ kernel (myproject_kernel.cpp)

        Args:
            model (ModelGraph): the hls4ml model.
        """

        filedir = os.path.dirname(os.path.abspath(__file__))
        f = open(os.path.join(filedir, '../templates/vitis_accelerator/myproject_kernel.cpp'))
        fout = open(f'{model.config.get_output_dir()}/{model.config.get_project_name()}_kernel.cpp', 'w')

        indent = '    '

        for line in f.readlines():
            if 'MYPROJECT' in line:
                newline = line.replace('MYPROJECT', format(model.config.get_project_name().upper()))
            elif 'myproject' in line:
                newline = line.replace('myproject', format(model.config.get_project_name()))
            else:
                newline = line
            fout.write(newline)

        f.close()
        fout.close()

    def write_host(self, model):
        """Write the Python-C++ kernel (myproject_host.cpp)

        Args:
            model (ModelGraph): the hls4ml model.
        """

        filedir = os.path.dirname(os.path.abspath(__file__))
        f = open(os.path.join(filedir, '../templates/vitis_accelerator/myproject_host.cpp'))
        fout = open(f'{model.config.get_output_dir()}/{model.config.get_project_name()}_host.cpp', 'w')

        indent = '    '

        for line in f.readlines():
            if 'MYPROJECT' in line:
                newline = line.replace('MYPROJECT', format(model.config.get_project_name().upper()))
            elif 'myproject' in line:
                newline = line.replace('myproject', format(model.config.get_project_name()))
            elif 'myproject_kernel' in line:
                newline = line.replace('myproject_kernel', format(model.config.get_project_name(), '_kernel'))
            else:
                newline = line
            fout.write(newline)

        f.close()
        fout.close()

    def write_makefile(self, model):
        """Write the Python-C++ Makefile (Makefile)

        Args:
            model (ModelGraph): the hls4ml model.
        """

        filedir = os.path.dirname(os.path.abspath(__file__))
        f = open(os.path.join(filedir, '../templates/vitis_accelerator/Makefile'))
        fout = open(f'./Makefile', 'w')

        indent = '    '

        for line in f.readlines():
            if 'MYPROJECT' in line:
                newline = line.replace('MYPROJECT', format(model.config.get_project_name().upper()))
            elif 'myproject' in line:
                newline = line.replace('myproject', format(model.config.get_project_name()))
            elif 'myproject_kernel' in line:
                newline = line.replace('myproject_kernel', format(model.config.get_project_name(), '_kernel'))
            else:
                newline = line
            fout.write(newline)

        f.close()
        fout.close()

    def write_accelerator_card_cfg(self, model):
        """Write the Python acceleratro card configuration (accelerator_card.cfg)

        Args:
            model (ModelGraph): the hls4ml model.
        """

        filedir = os.path.dirname(os.path.abspath(__file__))
        f = open(os.path.join(filedir, '../templates/vitis_accelerator/accelerator_card.cfg'))
        fout = open(f'{model.config.get_output_dir()}/accelerator_card.cfg', 'w')

        indent = '    '

        for line in f.readlines():
            if 'MYPROJECT' in line:
                newline = line.replace('MYPROJECT', format(model.config.get_project_name().upper()))
            elif 'myproject' in line:
                newline = line.replace('myproject', format(model.config.get_project_name()))
            elif 'myproject_kernel' in line:
                newline = line.replace('myproject_kernel', format(model.config.get_project_name(), '_kernel'))
            else:
                newline = line
            fout.write(newline)

        f.close()
        fout.close()
    
    def write_hls(self, model):
        """
        Write the HLS project. Calls the steps from VivadoWriter, adapted for Vitis
        """
        print("[K] Vitis_accelerator_writer -> write_hls called\n\n\n\n")
        super().write_hls(model)
        self.write_nnet_utils_overrides(model)
        self.write_build_script_backend_override(model)
        self.write_kernel(model)
        self.write_host(model)
        self.write_makefile(model)
        self.write_accelerator_card_cfg(model)
