import json
import os

import numpy as np

from hls4ml.model.layers import FixedPrecisionType, IntegerPrecisionType


class VitisAcceleratorConfig:
    def __init__(self, config):
        self.config = config.config

        self.platform = self.config['AcceleratorConfig'].get(
            'Platform', 'xilinx_u250_xdma_201830_2'
        )  # Get platform folder name

    def get_platform(self):
        return self.platform