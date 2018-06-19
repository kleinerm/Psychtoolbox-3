from distutils.core import setup, Extension

GetSecs = Extension('GetSecs',
                    extra_compile_args = ['-Wno-date-time'],
                    define_macros = [('PTBMODULE_GetSecs', None), ('PTBOCTAVE3MEX', None), ('PSYCH_LANGUAGE', 'PSYCH_PYTHON')],
                    include_dirs = ['Common/Base', 'Linux/Base', 'Common/GetSecs', 'Common/Screen'],
                    sources = ['Common/Base/MiniBox.c', 'Common/Base/PsychInstrument.c', 'Common/Base/ProjectTable.c', 'Common/Base/PsychError.c', 'Common/Base/PsychMemory.c',
                               'Common/Base/PsychHelp.c', 'Common/Base/PsychRegisterProject.c',  'Common/Base/PsychVersioning.c', 'Common/Base/PsychAuthors.c',
                               'Common/Base/PsychInit.c', 'Common/Base/MODULEVersion.c', 'Common/Base/PsychScriptingGlue.c', 'Common/Base/PythonGlue/PsychScriptingGluePython.c',
                               'Linux/Base/PsychTimeGlue.c',
                               'Common/GetSecs/GetSecs.c', 'Common/GetSecs/RegisterProject.c'],
                    libraries = ['c', 'rt']
                   )

setup (name = 'PackageName',
        version = '1.0',
        description = 'This is a demo package',
        ext_modules = [GetSecs])
