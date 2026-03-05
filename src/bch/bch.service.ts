import { Injectable } from '@nestjs/common';
import path from 'path';
import { BchAddon } from './interface/bch.interface';

// eslint-disable-next-line @typescript-eslint/no-unsafe-assignment, @typescript-eslint/no-require-imports
const bchAddon: BchAddon = require(
  path.join(process.cwd(), 'build/Release/bchaddon.node'),
);

@Injectable()
export class BchService {
  generateSyndrome(data: Buffer): number[] {
    return bchAddon.generateSyndrome(data);
  }

  recover(noisyData: Buffer, savedSyndromes: number[]): Buffer {
    return bchAddon.recover(noisyData, savedSyndromes);
  }

  // 여기서 실제 얼굴 이미지 -> 255bit/32바이트 전처리 결과를 만드는 로직을 넣으면 됨
  imageToBchInput(imageBuffer: Buffer): Buffer {
    // TODO:
    // 1. 얼굴 검출
    // 2. 특징 추출
    // 3. 이진화 / 255bit 생성
    // 4. Buffer 반환

    // 지금은 테스트용으로 앞 32바이트만 쓰는 예시
    const out = Buffer.alloc(32, 0);
    imageBuffer.copy(out, 0, 0, Math.min(imageBuffer.length, 32));
    return out;
  }
}
