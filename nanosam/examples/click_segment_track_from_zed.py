import PIL.Image
import cv2
import pyzed.sl as sl
import numpy as np
import argparse
import os
from nanosam.utils.predictor import Predictor
from nanosam.utils.tracker import Tracker

code_dir = os.path.dirname(os.path.realpath(__file__))
parser = argparse.ArgumentParser()
parser.add_argument("--image_encoder", type=str, default=f"{code_dir}/../data/resnet18_image_encoder.engine")
parser.add_argument("--mask_decoder", type=str, default=f"{code_dir}/../data/mobile_sam_mask_decoder.engine")
args = parser.parse_args()

def cv2_to_pil(image):
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    return PIL.Image.fromarray(image)

predictor = Predictor(
    args.image_encoder,
    args.mask_decoder
)

tracker = Tracker(predictor)

mask = None
point = None

# cap = cv2.VideoCapture(0)
zed = sl.Camera()
init_params = sl.InitParameters()
init_params.camera_resolution = sl.RESOLUTION.HD1080
init_params.camera_fps = 30

status = zed.open(init_params)
if status != sl.ERROR_CODE.SUCCESS:
    print("Camera Open : " + repr(status) + ". Exit program.")
    exit(1)
    
image = sl.Mat()
runtime = sl.RuntimeParameters()

def init_track(event,x,y,flags,param):
    global mask, point
    if event == cv2.EVENT_LBUTTONDBLCLK:
        mask = tracker.init(image_pil, point=(x, y))
        point = (x, y)


cv2.namedWindow('image')
cv2.setMouseCallback('image',init_track)

while True:
    if zed.grab(runtime) == sl.ERROR_CODE.SUCCESS:
        zed.retrieve_image(image, sl.VIEW.LEFT)
        img = image.get_data()
        img = cv2.resize(img, (1280,720))

        image_pil = cv2_to_pil(img)

        if tracker.token is not None:
            mask, point = tracker.update(image_pil)
    
        # Draw mask
        if mask is not None:
            bin_mask = (mask[0,0].detach().cpu().numpy() < 0)
            green_image = np.zeros_like(img)
            green_image[:, :] = (0, 185, 118, 125)
            green_image[bin_mask] = 0

            img = cv2.addWeighted(img, 0.4, green_image, 0.6, 0)

        # Draw center
        if point is not None:

            img = cv2.circle(
                img, point, 5, (0, 185, 118), -1
            )

        img = cv2.cvtColor(img, cv2.COLOR_RGBA2RGB)
        cv2.imshow("image", img)

        ret = cv2.waitKey(1)

        if ret == ord('s'):
            save_mask = (mask[0,0].detach().cpu().numpy() > 0)
            save_mask = (save_mask * 255).astype(np.uint8)
            cv2.imwrite('../FoundationPose/demo_data/grasp_tube0/masks/mask.png', save_mask)
            print('mask saved successfully!')
            break
        elif ret == ord('r'):
            tracker.reset()
            mask = None
            box = None

cv2.destroyAllWindows()
