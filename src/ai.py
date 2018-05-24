# coding: utf-8

from ogre import CharacterController
from re import match
from datetime import datetime, timedelta

################################################################################
# Command Definitions
#region Command definitions

class Command:
    args = ()

    def __init__(self, seconds=1):
        self.seconds = seconds

    @property
    def stop_at(self):
        return datetime.now() + timedelta(seconds=self.seconds)

    def execute(self, character):
        func = getattr(character, self.func_name)
        func(*self.args)

class run_forward(Command):
    func_name = "run"
    args = -1, 0

class run_backward(Command):
    func_name = "run"
    args = 1, 0

class stand(Command): 
    func_name = "stand"

class dance(Command):
    def execute(self, character):
        # TODO: Sinbad has to stand still before dancing
        #character.stand()
        character.dance()

class jump(Command):
    func_name = "jump"

class draw_swords(Command):
    func_name = "draw_swords"

class sheath_swords(Command):
    func_name = "sheath_swords"

#endregion
################################################################################


################################################################################
# Script for Sinbad

SCRIPT = [
    dance(seconds=4),
    stand(seconds=0.5),
    #draw_swords(),
    #run_forward(seconds=2),
    #jump(),
    #run_backward(seconds=2),
    #stand(seconds=0.1),
    #sheath_swords(),
]
################################################################################


################################################################################
# Callbacks from framework

def on_initialize(character : CharacterController, context_dict : dict):
    context_dict["stop_at"] = datetime.now() + timedelta(seconds=3)
    context_dict["script_line"] = -1


def on_frame(character : CharacterController, context_dict : dict, time_since_last_frame : float):
    if datetime.now() < context_dict["stop_at"]:
        return

    # Set a breakpoint here to step through every time the action changes
    i = (context_dict["script_line"] + 1) % len(SCRIPT)
    command = SCRIPT[i]
    context_dict["script_line"] = i
    context_dict["stop_at"] = command.stop_at
    
    command.execute(character)

################################################################################
