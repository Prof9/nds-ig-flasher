# NDS insideGadgets flasher

This is a Nintendo DS homebrew utility to flash GBA ROMs to insideGadgets GBA flash carts, if you don't want to buy a GBxCart RW.

Note: this is for flashing only; if you want to dump the ROM that's already on a GBA flash cart, use something like GBA Backup Tool.

## Usage

Name your ROM `flash.gba` and place it in a folder named `ig` on the root of your microSD card. Put the GBA flash cart in SLOT-2 and run the flasher.

## Supported flash carts

* insideGadgets GBA 32MB RTC 1Mbit Flash Save Flash Cart
* insideGadgets GBA 16MB 64K EEPROM Solar+RTC Flash Cart

Maybe some of their other carts too?

## Building

Install devkitPro, run `make`.

## Credits

* [nflsilva](https://github.com/nflsilva) for [nds-gbabf](https://github.com/nflsilva/nds-gbabf) which this was based off of.

## Disclaimer

This tool is not affiliated with or endorsed by insideGadgets.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

