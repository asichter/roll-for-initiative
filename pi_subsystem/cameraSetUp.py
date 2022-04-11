from picamera import PiCamera
from  time import sleep
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('Camera')
parser.add_argument('Die')
parser.add_argument('PicName')
args = parser.parse_args()
print(args.Camera)
print(args.Die)
camera = PiCamera()
camera.start_preview()
sleep(10)
camera.capture("/home/pi/Desktop/roll-for-initiative/pi_subsystem/TestingPictures/{module}/{die}/{name}.jpg".format(module=args.Camera,die=args.Die,name=args.PicName))
camera.stop_preview()
camera.close()