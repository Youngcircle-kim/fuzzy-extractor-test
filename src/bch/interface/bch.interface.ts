export interface BchAddon {
  generateSyndrome(data: Buffer): number[];
  recover(noisyData: Buffer, savedSyndromes: number[]): Buffer;
}
