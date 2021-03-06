/* aesrng.c - AES-based random number generator
 *
 * Copyright (c) 2009 Christopher Wellons <mosquitopsu@gmail.com>
 * Copyright (c) 2013 Sergey "Shnatsel" Davidoff <shnatsel@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for
 * any purpose with or without fee is hereby granted, provided that
 * the above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <gcrypt.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char **argv)
{
  size_t bytes = 1;
  if (argc > 1)
    bytes = atoi (argv[1]);

  /* Initialize the cipher */
  gcry_cipher_hd_t aes;
  gcry_cipher_open(&aes, GCRY_CIPHER_AES, GCRY_CIPHER_MODE_CBC, 0);

  /* Initialize the cipher key and iv */
  char key[32], iv[16];
  gcry_randomize (&key[0], 32, GCRY_STRONG_RANDOM);
  gcry_create_nonce (&iv[0], 16);
  gcry_cipher_setkey (aes, key, 32);
  gcry_cipher_setiv (aes, iv, 16);

  /* Initialize the counter */
/*
  unsigned char counter_iv[16];
  gcry_create_nonce (&counter_iv[0], 16);
  gcry_mpi_t counter = gcry_mpi_new (16);
  gcry_mpi_scan (&counter, GCRYMPI_FMT_USG, &counter_iv[0], 16, NULL);
*/

  /* Initialize the webcam */
  unsigned char webcam_data_raw[16];
  gcry_mpi_t webcam_data_mpi = gcry_mpi_new (16);
  int nread;
  int webcam_device = open("/dev/video0", O_RDONLY);
  if (webcam_device == -1) {
    fprintf(stderr, "Could not open webcam device!\n");
    return EXIT_FAILURE;
  }

  unsigned char in[16], out[16];
  while (bytes)
    {
      /* Read data from webcam */
      nread = read(webcam_device, webcam_data_raw, 16);
      if (nread == -1) {
        fprintf(stderr, "A read error has occurred!\n");
        return EXIT_FAILURE;
      }
      gcry_mpi_scan (&webcam_data_mpi, GCRYMPI_FMT_USG, &webcam_data_raw[0], 16, NULL);

      gcry_mpi_print (GCRYMPI_FMT_USG, &in[0], 16, NULL, webcam_data_mpi);
      gcry_cipher_encrypt (aes, &out[0], 16, &in[0], 16);

      /* Print out the bytes. */
      size_t nout = 16;
      if (nout > bytes)
        nout = bytes;
      fwrite (&out[0], nout, 1, stdout);

      bytes -= nout;
    }

  close(webcam_device);

  return EXIT_SUCCESS;
}
