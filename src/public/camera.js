export async function startCamera(videoEl) {
  const stream = await navigator.mediaDevices.getUserMedia({
    video: {
      facingMode: 'user',
      width: { ideal: 640 },
      height: { ideal: 480 },
    },
    audio: false,
  });

  videoEl.srcObject = stream;
  await videoEl.play();
  return stream;
}

export function captureFrame(videoEl, canvasEl) {
  const width = videoEl.videoWidth || 640;
  const height = videoEl.videoHeight || 480;

  canvasEl.width = width;
  canvasEl.height = height;

  const ctx = canvasEl.getContext('2d');
  ctx.drawImage(videoEl, 0, 0, width, height);
}

export function canvasToBlob(canvasEl, type = 'image/jpeg', quality = 0.95) {
  return new Promise((resolve) => {
    canvasEl.toBlob((blob) => resolve(blob), type, quality);
  });
}

export function stopCamera(stream) {
  if (!stream) return;
  stream.getTracks().forEach((track) => track.stop());
}
