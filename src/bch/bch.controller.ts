import {
  Controller,
  Post,
  UploadedFile,
  UseInterceptors,
  BadRequestException,
} from '@nestjs/common';
import { FileInterceptor } from '@nestjs/platform-express';
import { memoryStorage } from 'multer';
import { BchService } from './bch.service';

@Controller('api/bch')
export class BchController {
  constructor(private readonly bchService: BchService) {}

  // local-DB
  private savedSyndromes: number[] | null = null;

  @Post('register')
  @UseInterceptors(
    FileInterceptor('image', {
      storage: memoryStorage(),
    }),
  )
  register(@UploadedFile() file: Express.Multer.File) {
    if (!file) {
      throw new BadRequestException('이미지 파일이 필요합니다.');
    }

    const bchInput = this.bchService.imageToBchInput(file.buffer);
    const syndrome = this.bchService.generateSyndrome(bchInput);

    this.savedSyndromes = syndrome;

    return {
      message: '등록 완료',
      inputHex: bchInput.toString('hex'),
      syndrome,
    };
  }

  @Post('verify')
  @UseInterceptors(
    FileInterceptor('image', {
      storage: memoryStorage(),
    }),
  )
  verify(@UploadedFile() file: Express.Multer.File) {
    if (!file) {
      throw new BadRequestException('이미지 파일이 필요합니다.');
    }

    if (!this.savedSyndromes) {
      throw new BadRequestException('먼저 등록을 진행하세요.');
    }

    const noisyInput = this.bchService.imageToBchInput(file.buffer);
    const recovered = this.bchService.recover(noisyInput, this.savedSyndromes);

    return {
      message: '검증 완료',
      noisyHex: noisyInput.toString('hex'),
      recoveredHex: recovered.toString('hex'),
      matched: recovered.equals(noisyInput), // 필요에 따라 비교 기준 변경
    };
  }
}
