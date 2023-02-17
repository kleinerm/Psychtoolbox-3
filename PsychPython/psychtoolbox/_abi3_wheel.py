# Copyright 2022, Joe Rickerby
# Copyright 2022, Alex Forrence
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from wheel.bdist_wheel import bdist_wheel

# derived from:
# https://github.com/joerick/python-abi3-package-sample/blob/7f05b22b9e0cfb4e60293bc85252e95278a80720/setup.py#L5
class abi3_wheel(bdist_wheel):

    def get_tag(self):
        py, abi, plat = super().get_tag()
        if py.startswith('cp'):
            # We support the limited API, compatible back to 3.7
            return 'cp37', 'abi3', plat
        return py, abi, plat
