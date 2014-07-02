from funq_server.runner import RunnerInjector

class LinuxRunnerInjector(RunnerInjector):
    def __init__(self, library_path, args, env):
        RunnerInjector.__init__(self, library_path, args, env)
        if 'LD_PRELOAD' in env:
            env['LD_PRELOAD'] +=  ' ' + self.library_path
        else:
            env['LD_PRELOAD'] =  self.library_path
        
