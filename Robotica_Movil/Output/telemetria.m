% Función para mostrar telemetría de robot móvil
% Laboratorio de Robótica 4º GIERM 18-19
% Federico Cuesta
% USO: Pasar como parámetro el nombre del archivo de telemetría con el
% sigiente contenido por fila:
% -Incremento de tiempo transcurrido desde la lectura anterior (milisegundos),  
% -Distancia medida por sensor Izq/Frontal (cm), 
% -Distancia medida por sensor Dch/Trasero(cm), 
% -Referencia de control (cm), 
% -Modo activo (0: Parado, 1: Control frontal, … 4),
% -Velocidad PWM motor Izq (+/-255, negativo indica marcha atrás), 
% -Velocidad PWM motor Dch (+/-255, negativo indica marcha atrás).

function telemetria(archivo)
close all;

tel=load(archivo);
muestras=length(tel);
disp('Incremento de tiempo mínimo:'); disp(min(tel(:,1)));
disp('Incremento de tiempo máximo:');disp(max(tel(:,1)));
disp('Incremento de tiempo promedio:'); disp(mean(tel(:,1)));
tiempo=zeros(1,muestras);
tiempo(1)=tel(1,1); %Vector de tiempo absoluto
for i=2:muestras
    tiempo(i)=tiempo(i-1)+tel(i,1);
end    

figure(1);
    subplot(2,1,1);
    plot(tiempo,tel(:,8)/10, 'b', tiempo,tel(:,4)/10, 'r');
    ylabel('Distancia hasta la pared (cm)');
    title('Control en distancia');
    legend('Distancia','Referencia');
    grid;

    subplot(2,1,2);
    plot(tiempo,tel(:,5), 'k', tiempo,tel(:,6), 'r', tiempo,tel(:,7), 'b');
    title('Actuadores');
    xlabel('Tiempo (ms)');
    ylabel('Señal de control (PWM)');
    legend('Modo', 'u_I', 'u_D');
    grid;
    aux = axis;
    aux(1,3) = -255;
    aux(1,4) = 255;
    axis(aux);

figure(2);
    subplot(2,1,1);
    plot(tiempo,tel(:,9)/10, 'b', tiempo,tel(:,10)/10, 'r');
    ylabel('Distancia diferencial (cm)');
    title('Control de angulo');
    legend('Distancia Diferencial', 'Referencia');
    grid;

    subplot(2,1,2);
    plot(tiempo,tel(:,5), 'k', tiempo,tel(:,6), 'r', tiempo,tel(:,7), 'b');
    title('Actuadores');
    xlabel('Tiempo (ms)');
    ylabel('Señal de control (PWM)');
    legend('Modo', 'u_I', 'u_D');
    grid;
    aux = axis;
    aux(1,3) = -255;
    aux(1,4) = 255;
    axis(aux);

figure(3);
    subplot(2,1,1);
    plot(tiempo,tel(:,2)/10, tiempo,tel(:,3)/10, tiempo,tel(:,4)/10);
    title('Lecturas de lo Sensores');
    ylabel('Distancia (cm)');
    legend('Ultrasonidos Izquierdo', 'Ultrasonidos Derecho', 'Referencia' );
    grid;
    
    subplot(2,1,2);
    plot(tiempo,tel(:,5), tiempo,tel(:,6), tiempo,tel(:,7));
    title('Actuadores');
    xlabel('Tiempo (ms)');
    ylabel('Señal de control (PWM)');
    legend('Modo', 'u_I', 'u_D');
    grid;
    aux = axis;
    aux(1,3) = -255;
    aux(1,4) = 255;
    axis(aux);

end