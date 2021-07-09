var init = false;
var gameBooted = false;

var imagedata;
var context;
var gamepad;

var fpsCounterElement = null;
var lastFrames = [0, 0];

var gamepadConnected = false;
var gamepadButtons = null;
var gamepadAxes = null;
const gamepadAxeThreshold = 0.45;
const buttonMapping = {'DPad-Up': 0,'DPad-Down': 1,'DPad-Left': 2,'DPad-Right': 3,'Start': 4,'Select': 5,'A': 6,'B': 7,}
const xOneButtonMapping = {12:'DPad-Up',13:'DPad-Down',14:'DPad-Left',15:'DPad-Right',9:'Start',8:'Select',1:'A',0:'B'}

window.setInterval(function(){
  displayFPS();
}, 300);


Module.onRuntimeInitialized = function() {

  var canvas = document.getElementById("game-canvas");
  context = canvas.getContext("2d");

  canvas.width = 160;
  canvas.height = 144;

 imagedata = context.createImageData(160, 144);

  Module._init();
  init = true;

  window.requestAnimationFrame(mainLoop);
}

function mainLoop()
{
  updateGamepadInputs();
  if(gameBooted) {
    Module._updateState();
    updateCanvas();
  }
  lastFrames[1] = lastFrames[0];
  lastFrames[0] = Date.now();
  
  // displayCPUInfo();
  window.requestAnimationFrame(mainLoop);
}

function displayFPS() {
    if(fpsCounterElement !== null && lastFrames[0] !== 0)
    {
        let delta = (lastFrames[0] - lastFrames[1]) / 1000;
        fps = 1 / delta;
        fpsCounterElement.textContent = Math.round(fps);
    }
}

function updateCanvas() {
    let ptr = Module._getCanvasImageData();
  
    for (var x = 0; x < 160; x++) {
      for (var y = 0; y < 144; y++) {
          // Get the pixel index
          var pixelindex = (y * 160 + x) * 4;
  
          // Set the pixel data
          imagedata.data[pixelindex] = Module.HEAPU8[ptr + pixelindex];
          imagedata.data[pixelindex + 1] = Module.HEAPU8[ptr + pixelindex + 1];
          imagedata.data[pixelindex + 2] = Module.HEAPU8[ptr + pixelindex + 2];
          imagedata.data[pixelindex + 3] = Module.HEAPU8[ptr + pixelindex + 3];
      }
    }

    Module._free(ptr);
    context.putImageData(imagedata, 0, 0);
}

function resizeCanvas() {
    var wrapper = document.getElementById('game-wrapper');
    var canvas = document.getElementById('game-canvas');
    var canvasRatio = 9 / 10;
    var windowRatio = wrapper.offsetHeight / wrapper.offsetWidth;
    var width;
    var height;

    if (windowRatio < canvasRatio) {
        height = wrapper.offsetHeight;
        width = height / canvasRatio;
    }
    else {
        width = wrapper.offsetWidth;
        height = width * canvasRatio;
    }

    canvas.style.width = width + 'px';
    canvas.style.height = height + 'px';
}

function gamepadHandler(event, connecting) {
  let newGamepad = event.gamepad;

  if (connecting) {
      gamepadConnected = true;
      gamepadButtons = new Array(newGamepad.buttons.length).fill(0);
      gamepadAxes = new Array(4).fill(0);
  }
  else {
      gamepadConnected = false;
      gamepadButtons = null;
      gamepadAxes = null;
  }
}

function keycodeFromGamepadIndex(buttonIndex){
  let button = buttonMapping[xOneButtonMapping[buttonIndex]];
  switch(button){
      case 7: return 90;
      case 6: return 88;
      case 4: return 8;
      case 5: return 32;
      case 0: return 38;
      case 1: return 40;
      case 2: return 37;
      case 3: return 39;
      default: return -1;
  }
}

function updateGamepadInputs() {
  if(gamepadConnected && init) {
      const gamepad = navigator.getGamepads()[0];
      //buttons
      gamepad.buttons.map(e => e.pressed).forEach((isPressed, buttonIndex) => {
          if(isPressed && gamepadButtons[buttonIndex] === 0) {
              let keycode = keycodeFromGamepadIndex(buttonIndex);
              if(keycode !== -1) {
                  gamepadButtons[buttonIndex] = 1;
                  Module._handleInput(keycode, true);
              }
          }
          else if(!isPressed && gamepadButtons[buttonIndex] === 1) {
              let keycode = keycodeFromGamepadIndex(buttonIndex);
              if(keycode !== -1) {
                  Module._handleInput(keycode, false);
                  gamepadButtons[buttonIndex] = 0;
              }
          }
      });
      //right
      if (gamepad.axes[0] > gamepadAxeThreshold && gamepadAxes[0] === 0) {
          Module._handleInput(39, true);
          gamepadAxes[0] = 1;
      }
      else if(gamepad.axes[0] < gamepadAxeThreshold && gamepadAxes[0] === 1) {
          Module._handleInput(39, false);
          gamepadAxes[0] = 0;
      }
      //left
      if (gamepad.axes[0] < -gamepadAxeThreshold && gamepadAxes[1] === 0) {
          Module._handleInput(37, true);
          gamepadAxes[1] = 1;
      }
      else if(gamepad.axes[0] > -gamepadAxeThreshold && gamepadAxes[1] === 1) {
          Module._handleInput(37, false);
          gamepadAxes[1] = 0;
      }
      //down
      if (gamepad.axes[1] > gamepadAxeThreshold && gamepadAxes[2] === 0) {
          Module._handleInput(40, true);
          gamepadAxes[2] = 1;
      }
      else if(gamepad.axes[1] < gamepadAxeThreshold && gamepadAxes[2] === 1) {
          Module._handleInput(40, false);
          gamepadAxes[2] = 0;
      }
      //up
      if (gamepad.axes[1] < -gamepadAxeThreshold && gamepadAxes[3] === 0) {
          Module._handleInput(38, true);
          gamepadAxes[3] = 1;
      }
      else if(gamepad.axes[1] > -gamepadAxeThreshold && gamepadAxes[3] === 1) {
          Module._handleInput(38, false);
          gamepadAxes[3] = 0;
      }
  }
}

window.addEventListener("load",function() {

  fpsCounterElement = document.getElementById("fps-counter");
  resizeCanvas();

  if (!init) return;

  document.onkeydown = function(e) {
      Module._handleInput(e.keyCode, true);
  }

  document.onkeyup = function(e) {
      Module._handleInput(e.keyCode, false);
  }

  //Init dropdown items
  let dropdownItems = document.getElementsByClassName("dropdown-title");
  for(let i = 0, len = dropdownItems.length; i < len; i++)
  {
    dropdownItems[i].onclick = function ()
    {
      let sibling = dropdownItems[i].nextElementSibling;
      if(sibling.classList.contains("hidden"))
      {
        sibling.classList.remove("hidden");
        dropdownItems[i].getElementsByTagName("i")[0].innerHTML = "keyboard_arrow_up";
      }
      else
      {
        sibling.classList.add("hidden");
        dropdownItems[i].getElementsByTagName("i")[0].innerHTML = "keyboard_arrow_down";
      }
    }
  }

  //load game button
  document.getElementById("pick-game-input").onchange = function(elem)
  {
    let input = document.getElementById("pick-game-input");
    if(input.value === "")
    {
      document.getElementById("pick-game-label").innerText = "Pick game"

    }
    else
    {
      document.getElementById("pick-game-label").innerText = input.value.substring(input.value.lastIndexOf('\\') + 1);
    }
  }

  document.getElementById("load-game-button").onclick = function(elem)
  {
    let fileToLoad = document.getElementById("pick-game-input").files[0];
    let fileReader = new FileReader();
    fileReader.onload = function(fileLoadedEvent){
      let byteArray = new Uint8Array(fileLoadedEvent.target.result);
      let heapSpace = Module._malloc(byteArray.length * byteArray.BYTES_PER_ELEMENT); 
      Module.HEAPU8.set(byteArray, heapSpace);
      Module._loadRom(heapSpace, byteArray.length);
      Module._free(heapSpace);
    };
    fileReader.readAsArrayBuffer(fileToLoad);
    //Module._resetCPU();
    gameBooted = true;
  }

  document.getElementById("classic-palette-input").onclick = function()
  {
    let div = document.getElementById("classic-palette-switch");
    if(div.classList.contains('mdc-switch--checked')) {
      div.classList.remove('mdc-switch--checked');
    }
    else {
      div.classList.add('mdc-switch--checked');
    }
    Module._switchPalette();
  }

  //Cpu speed input
  document.getElementById("cpu-speed-input").oninput = function()
  {
    Module._setCPUSpeed(document.getElementById("cpu-speed-input").value);
  }
}, false);

window.addEventListener("resize",function() {
  resizeCanvas();
});

window.addEventListener("gamepadconnected", function(e) { gamepadHandler(e, true); }, false);
window.addEventListener("gamepaddisconnected", function(e) { gamepadHandler(e, false); }, false);