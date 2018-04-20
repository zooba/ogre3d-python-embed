# coding: utf-8

from ogre import CharacterController
from re import match
from datetime import datetime, timedelta

SCRIPT = [
    'run forward for 2 seconds',
    'dance for 4 seconds',
    'stand for 0.5 seconds',
    # TODO: Show Sinbad's swords
    #'draw swords',
    #'slice',
    #'slice vertically',
    #'stand for 1 second',
    #'sheath swords',
    'run backward for 2 seconds',
    'jump',
]

def read_duration(s):
    m = match(r'.*for ([\d.]+) seconds?$', s)
    if m:
        return timedelta(seconds=float(m.group(1)))
    m = match(r'.*for (\d+) milliseconds?$', s)
    if m:
        return timedelta(seconds=float(m.group(1)) / 1000)

    # We don't know how long, so run for one second
    return timedelta(seconds=1)


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
    context_dict["stop_at"] = datetime.now() + read_duration(command)
    
    if command.startswith('run forward'):
        character.run(-1, 0)

    if command.startswith('run backward'):
        character.run(1, 0)

    if command.startswith('stand'):
        character.stand()

    if command.startswith('dance'):
        # TODO: Sinbad cannot dance unless he's standing still
        #character.stand()
        character.dance()

    if command.startswith('jump'):
        character.jump()

    if command.startswith('draw sword'):
        character.draw_swords()

    if command.startswith('slice vertical'):
        character.slice(True)
    elif command.startswith('slice'):
        character.slice(False)

    if command.startswith('sheath sword'):
        character.sheath_swords()
