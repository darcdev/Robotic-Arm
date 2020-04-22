/*******************************************/
/*      |__Proyecto Brazo Robotico__|      */
/*      ----------Autores-----------       */
/*      | Yuberley Guerrero Castro |       */
/*      |   Sebastian Baez Ramos   |       */
/*      |   Roger Ricardo Roldan   |       */
/*      |   Diego Cabrera Rojas    |       */
/*      ----------------------------       */
/*      _°Universidad De Los Llanos°_      */
/*     ©Todos los Derechos Reservados      */
/*******************************************/

#include <18f8722.h>
#fuses xt, nowdt, nomclr
//activar conversor analogico-digital
#device ADC = 10
//habilitar reloj interno para uso de servomotores
#use delay(internal = 500K)

//configuracion puerto C, pines asociados => [boton automatico , boton manual]
#byte portb = 0xf81
//lecturas de conversion ADC de los potenciometros
int16 lecturasADC[5] = {
  0,
  0,
  0,
  0,
  0
};
int8 valorBitsGrado;
int8 estado = 0;
int8 activarBotonesGrabar = 0;
//guardar posiciones de servomotores en instante de tiempo especifico , diferentes combinaciones
//se graban 4 movimientos
int16 grabacion1[5] = {
  0,
  0,
  0,
  0,
  0
};
int16 grabacion2[5] = {
  0,
  0,
  0,
  0,
  0
};
int16 grabacion3[5] = {
  0,
  0,
  0,
  0,
  0
};
int16 grabacion4[5] = {
  0,
  0,
  0,
  0,
  0
};
//variables de verificacion si se pulso el boton de grabacion N
int8 grabarPosicion1 = 0;
int8 grabarPosicion2 = 0;
int8 grabarPosicion3 = 0;
int8 grabarPosicion4 = 0;
/**
 *grabar la posicion actual de servomotores dependiendo del pulsador presionado [rec1,rec2,rec3,rec4]
 *
 *@param grabacionN - lista a llenar de posiciones servomotores en instante de tiempo especifico
 **/
void grabarPosiciones(int16 grabacionN[]) {
  int8 i;
  for (i = 0; i < 5; i++) {
    grabacionN[i] = lecturasADC[i];
  }
}
/**
 *Interrupcion para movimiento de grabacion 1
 *
 *@param void 
 *@see int_ext - Activacion de interrupcion pin rb0
 **/
#int_ext
void REC1(void) {
  //activar efecto antirebote
  while (input(pin_b0)) {}
  if(activarBotonesGrabar == 1){
  grabarPosicion1 = 1;
  //grabar primera posicion
  grabarPosiciones(grabacion1);
  }
  
}
/**
 *Interrupcion para  movimiento de grabacion 2
 *
 *@param void 
 *@see int_ext1 - Activacion de interrupcion pin rb1
 **/
#int_ext1
void REC2(void) {
  //activar efecto antirebote
  while (input(pin_b1)) {}
  if(activarBotonesGrabar == 1 && grabarPosicion1 == 1 ){
  grabarPosicion2 = 1;
  //grabar segunda posicion
  grabarPosiciones(grabacion2);
  }
}
/**
 *Interrupcion para movimiento de grabacion 3
 *
 *@param void 
 *@see int_ext2 - Activacion de interrupcion pin rb2
 **/
#int_ext2
void REC3(void) {
  //activar efecto antirebote
  while (input(pin_b2)) {}
  if(activarBotonesGrabar == 1 && grabarPosicion2 == 1 ){
  grabarPosicion3 = 1;
  //grabar tercera posicion
  grabarPosiciones(grabacion3);
  }
}
/**
 *Interrupcion para  movimiento de grabacion 4
 *
 *@param void 
 *@see int_ext3 - Activacion de interrupcion pin rb3
 **/
#int_ext3
void REC4(void) {
  //activar efecto antirebote
  while (input(pin_b3)) {}
  if(activarBotonesGrabar == 1 && grabarPosicion3 == 1){
  grabarPosicion4 = 1;
  //grabar cuarta posicion
  grabarPosiciones(grabacion4);
  }
}
/**
 *Cambiar los grados de servomotoyr respectivo pasado por parametro
 *
 *@param servomotor - numero del servomotor a cambiar los grados
 **/
void cambiarGradoServomotor(int16 listaAConvertir[],int8 servomotor) {
  switch (servomotor) {
    //Grado Servomotor Cadera
  case 0:
    //ecuacion conversion grado a bits para duty_cicle (-90°,90°)
    valorBitsGrado = (60 * listaAConvertir[servomotor] / 1023) + 17;
    set_pwm1_duty((int16) valorBitsGrado);
    break;
    //Grado Servomotor Codo
  case 1:
    //ecuacion conversion grado a bits para duty_cicle (-90°,0°)
    valorBitsGrado = ((30 * listaAConvertir[servomotor]) / 1023) + 17;
    set_pwm2_duty((int16) valorBitsGrado);
    break;
    //Grado Servomotor Muñeca
  case 2:
    //ecuacion conversion grado a bits para duty_cicle (-90°,0°)
    valorBitsGrado = (30 * listaAConvertir[servomotor] / 1023) + 17;
    set_pwm3_duty((int16) valorBitsGrado);
    break;
    //Grado Servomotor Mano
  case 3:
    //ecuacion conversion grado a bits para duty_cicle (-45°,0°)
    valorBitsGrado = (15 * listaAConvertir[servomotor] / 1023) + 32;
    set_pwm4_duty((int16) valorBitsGrado);
    break;
    //Grado Servomotor hombro
  case 4:
    //ecuacion conversion grado a bits para duty_cicle (-45°,45°)
    valorBitsGrado = (30 * listaAConvertir[servomotor] / 1023) + 32;
    set_pwm5_duty((int16) valorBitsGrado);
    break;

  }
}
void main() {

  //Habilitar entradas o salidas puertos
  set_tris_a(0b11111111);
  set_tris_b(0b11111111);
  set_tris_c(0b00000001);

  //Señal de periodo para servomotores __20ms__
  setup_timer_2(T2_DIV_BY_16, 155, 1);

  //Configuracion pwm de los servomotores
  setup_ccp1(ccp_pwm);
  setup_ccp2(ccp_pwm);
  setup_ccp3(ccp_pwm);
  setup_ccp4(ccp_pwm);
  setup_ccp5(ccp_pwm);

  //Configuracion del conversor ADC
  setup_adc_ports(all_analog);
  setup_adc(adc_clock_internal);

  //Configuracion Interrupciones
  enable_interrupts(int_ext);
  ext_int_edge(0, l_to_h);
  enable_interrupts(int_ext1);
  ext_int_edge(1, l_to_h);
  enable_interrupts(int_ext2);
  ext_int_edge(2, l_to_h);
  enable_interrupts(int_ext3);
  ext_int_edge(3, l_to_h);
  enable_interrupts(global);

  while (true) {
    //captura el estado(modo) actual opc -> manual | automatico
    //manual -> 0 || automatico -> 1
    estado = input(PIN_C0);
    //si esta activo el control manual
    if (estado == 0) {
      activarBotonesGrabar = 1;
      int8 i;
      //recorrer todas las lecturas de los potenciometros
      for (i = 0; i < 5; i++) {
        //establecer el canal de conversion del ADC
        set_adc_channel(i);
        //retardo para conversion sin fallas
        delay_ms(1);
        //lectura almacenada para lectura de cada potenciometro
        lecturasADC[i] = read_adc();
        //cambiar grado servomotor respectivo 
        cambiarGradoServomotor(lecturasADC,i);
      }
      //retardo para evitar parpadeo servomotores
      delay_ms(100);
    }
    //si esta activo el control automatico
    else if (estado == 1) {
      activarBotonesGrabar = 0;
      //si llego al estado manual prender led de aviso
      //output_high(pin_c3);
      
      //determinar cantidad de posiciones activadas
      int8 sumaBotonesActivos = grabarPosicion1 + grabarPosicion2 + grabarPosicion3 + grabarPosicion4;  
      if(sumaBotonesActivos >= 1 ) {
      int8 i;
      //recorrer todas las posiciones preestablecidas por usuario
      for(i = 1 ; i <= sumaBotonesActivos ; i++){
        int8 j;
        //mover servomotores correspondientes por posición
        for(j = 0; j < 5 ; j++){
           //determina el servomotor que debe girar en la posicion correcta
           switch(i){
             case 1:
               cambiarGradoServomotor(grabacion1 , j);
             break;
             case 2:
               cambiarGradoServomotor(grabacion2 , j);
             break;
             case 3:
               cambiarGradoServomotor(grabacion3 , j);
             break;
             case 4:
               cambiarGradoServomotor(grabacion4 , j);
             break;
           } 
        }
        //tiempo de retardo cambio de posicion
        delay_ms(1500);
      
      }
      }else if(sumaBotonesActivos == 0){
         //secuencia de pasos cuando no haya sido definida por usuario
      } 
    }

  }
}
