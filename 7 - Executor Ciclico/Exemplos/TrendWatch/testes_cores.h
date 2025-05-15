/**
 * ------------------------------------------------------------
 *  Arquivo: testes_cores.h
 *  Projeto: TempCycleDMA
 * ------------------------------------------------------------
 *  Descrição:
 *      Define constantes simbólicas para cores RGB usadas
 *      com a matriz NeoPixel.
 *
 *      As cores seguem intensidade moderada para segurança
 *      visual (até 64).
 *
 *  
 *  Data: 12/05/2025
 * ------------------------------------------------------------
 */

#ifndef TESTE_CORES_H
#define TESTE_CORES_H

// Intensidade
#define COR_APAGA  0
#define COR_MAX    64
#define COR_MEIA   32

// Cores RGB simbólicas
#define COR_R      COR_MAX, COR_APAGA, COR_APAGA
#define COR_G      COR_APAGA, COR_MAX, COR_APAGA
#define COR_B      COR_APAGA, COR_APAGA, COR_MAX
#define COR_X COR_MAX, COR_MAX, COR_MAX

#define COR_VERMELHO  COR_R
#define COR_VERDE     COR_G
#define COR_AZUL      COR_B
#define COR_BRANCA    COR_X

#define LED_PIN 7

// Funções de teste (opcional)
void preencher_matriz_com_cores(void);
void testar_fileiras_colunas(void);

#endif
