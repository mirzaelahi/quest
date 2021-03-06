%
% Copyright (C) 2014  K M Masum Habib <masum.habib@gmail.com>
%
function out = importTransResult(fileName)
    fid = fopen(fileName, 'rt');
    out = [];
    ibIE = 1;
    ibn = 1;
    ibneq = 1;
    while (~feof(fid))
        type = fscanf(fid, '%s[^\n]');
        if strfind(type, 'ENERGY') == 1
            [out.NE, out.E] = scanE();
        elseif strfind(type, 'TRANSMISSION') == 1
            out.TE = scan();
        elseif strfind(type, 'CURRENT') == 1
            out.IE{ibIE} = scan();
            ibIE = ibIE + 1;
        elseif strfind(type, 'DOS') == 1
            out.DOS = scan();
        elseif strfind(type, 'n') == 1
            out.n{ibn} = scan();
            ibn = ibn + 1;
        elseif strfind(type, 'neq') == 1
            out.neq{ibneq} = scan();
            ibneq = ibneq + 1;
        end
    end
    
    fclose(fid);
    
    function M = scan()
        NE = fscanf(fid, '%d[^\n]');
        %M.NE = NE;
        tmp = fscanf(fid, '%d %d[^\n]');
        M.ib = tmp(1); M.jb = tmp(2);
        N = fscanf(fid, '%d[^\n]');
        %M.N = N;
        for iE = 1:NE
            %M.E(iE) = fscanf(fid, '%f[^\n]');
            M.M{iE} = zeros(N,N);
            for ii = 1:N
                for jj = 1:N
                    data = fscanf(fid, '%*[ \n\t](%e,%e)', 2);
                    M.M{iE}(ii,jj) = data(1) + 1i*data(2);
                end
            end
        end    
    end

    function [NE, EE] = scanE()
        NE = fscanf(fid, '%d[^\n]');
        EE = zeros(NE, 1);
        for iE = 1:NE
            EE(iE) = fscanf(fid, '%f[^\n]');
        end    
    end
end
    
