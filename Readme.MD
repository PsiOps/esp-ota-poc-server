Install Compilation stuff
- Install AWS CLI (not needed for Lambda)
- Install arduino CLI
	`curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh`
- Add path to PATH: https://askubuntu.com/a/170240/545615
- Create a arduino-cli.yaml config file
	`arduino-cli config init` => /home/ubuntu/.arduino15/arduino-cli.yaml
- Install additional boards:
	editing the yaml: `sudo nano /home/ubuntu/.arduino15/arduino-cli.yaml`
````
board_manager:
  additional_urls:
    - http://arduino.esp8266.com/stable/package_esp8266com_index.json
````

	update the index: `arduino-cli core update-index`
	then install the platform: `arduino-cli core install esp8266:esp8266`

- Download extra libraries to src folder of compile dir
- Compile the sketch:
	`arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 ~/TestSketch/TestSketch.ino`
	
- Upload it to S3
	`aws s3 cp ~/TestSketch/TestSketch.ino.esp8266.esp8266.nodemcuv2.bin s3://esp-ota-binaries/test-binary.bin`
	
- Optional: Test the cli-compiled thing locally by flashing the thing manually:
	`C:\Users\Paul.siersma\AppData\Local\Arduino15\packages\esp8266\tools\python\3.7.2-post1/python C:\Users\Paul.siersma\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.5.2/tools/upload.py --chip esp8266 --port COM13 --baud 115200 --trace version --end --chip esp8266 --port COM13 --baud 115200 --trace write_flash 0x0 C:\Users\PAUL~1.SIE\Documents\Arduino\binary.bin --end`

Create express API to handle POST call to compile
- install git
- install node
- install npm
- Develop the server app
	https://stackoverflow.com/questions/20643470/execute-a-command-line-binary-with-node-js
- install npm packages
- run the server app
- test with postman



